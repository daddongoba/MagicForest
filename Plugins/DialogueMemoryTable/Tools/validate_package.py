#!/usr/bin/env python3
"""Dependency-free package checks; this is not a substitute for an Unreal build."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
STABLE_KEY = re.compile(r"^[a-z0-9_.-]{1,64}$")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def load_json(relative_path: str) -> object:
    with (ROOT / relative_path).open("r", encoding="utf-8") as handle:
        return json.load(handle)


def check_balanced_cpp(path: Path) -> None:
    text = path.read_text(encoding="utf-8")
    stack: list[tuple[str, int]] = []
    pairs = {")": "(", "]": "[", "}": "{"}
    state = "code"
    index = 0

    while index < len(text):
        char = text[index]
        next_char = text[index + 1] if index + 1 < len(text) else ""

        if state == "code":
            if char == "/" and next_char == "/":
                state = "line_comment"
                index += 2
                continue
            if char == "/" and next_char == "*":
                state = "block_comment"
                index += 2
                continue
            if char == '"':
                state = "string"
            elif char == "'":
                state = "character"
            elif char in "([{":
                stack.append((char, index))
            elif char in ")]}":
                require(bool(stack), f"{path.name}: unexpected {char} at {index}")
                opening, opening_index = stack.pop()
                require(opening == pairs[char], f"{path.name}: mismatched {opening} at {opening_index}")
        elif state == "line_comment":
            if char == "\n":
                state = "code"
        elif state == "block_comment":
            if char == "*" and next_char == "/":
                state = "code"
                index += 2
                continue
        elif state in {"string", "character"}:
            if char == "\\":
                index += 2
                continue
            if (state == "string" and char == '"') or (state == "character" and char == "'"):
                state = "code"

        index += 1

    require(state in {"code", "line_comment"}, f"{path.name}: unterminated literal or comment")
    if stack:
        raise AssertionError(f"{path.name}: unclosed delimiter {stack[-1]}")


def check_sample(sample: dict[str, object]) -> None:
    root_fields = {
        "schema_version",
        "npc_id",
        "session_id",
        "npc_patch",
        "fact_upserts",
        "world_suggestions",
        "open_loop_upserts",
    }
    require(set(sample) == root_fields, "sample root fields differ from the runtime contract")
    require(sample["schema_version"] == 1, "sample schema_version must be 1")
    require(sample["npc_id"] in {"blacksmith", "operator", "novelist"}, "unsupported sample npc_id")
    require(isinstance(sample["session_id"], int) and sample["session_id"] >= 1, "invalid sample session_id")

    npc_patch = sample["npc_patch"]
    require(isinstance(npc_patch, dict), "npc_patch must be an object")
    require(npc_patch["attitude"] in {"guarded", "neutral", "warm", "hurt", "hostile"}, "invalid attitude")
    require(-10 <= npc_patch.get("trust_delta_suggested", 0) <= 10, "invalid trust delta")
    require(len(npc_patch["topic_tags"]) <= 8, "too many topic tags")

    facts = sample["fact_upserts"]
    world = sample["world_suggestions"]
    loops = sample["open_loop_upserts"]
    require(isinstance(facts, list) and len(facts) <= 6, "invalid fact_upserts")
    require(isinstance(world, list) and len(world) <= 3, "invalid world_suggestions")
    require(isinstance(loops, list) and len(loops) <= 3, "invalid open_loop_upserts")

    for row in facts:
        require(bool(STABLE_KEY.fullmatch(row["fact_key"])), "invalid fact_key")
        require(row["authority"] in {"npc_disclosure", "npc_report", "inference"}, "invalid fact authority")
    for row in world:
        require(bool(STABLE_KEY.fullmatch(row["state_key"])), "invalid state_key")
        require(row["authority"] in {"npc_report", "inference"}, "invalid world authority")
    for row in loops:
        require(bool(STABLE_KEY.fullmatch(row["loop_key"])), "invalid loop_key")


def main() -> int:
    descriptor = load_json("DialogueMemoryTable.uplugin")
    schema = load_json("Resources/DialogueMemoryPatch.schema.json")
    sample = load_json("Resources/ExampleMemoryPatch.json")

    require(descriptor["FileVersion"] == 3, "unexpected plugin descriptor version")
    require(descriptor["Modules"][0]["Name"] == "DialogueMemoryTable", "module name mismatch")
    require(schema["title"] == "DialogueMemoryPatch", "schema title mismatch")
    check_sample(sample)

    source_files = [
        ROOT / "Source/DialogueMemoryTable/Public/DialogueMemoryTypes.h",
        ROOT / "Source/DialogueMemoryTable/Public/DialogueMemoryTableSubsystem.h",
        ROOT / "Source/DialogueMemoryTable/Public/AlchemyGameplayTypes.h",
        ROOT / "Source/DialogueMemoryTable/Public/AlchemyGameplaySubsystem.h",
        ROOT / "Source/DialogueMemoryTable/Public/DialogueAIServiceSubsystem.h",
        ROOT / "Source/DialogueMemoryTable/Public/DailyMushroomSubsystem.h",
        ROOT / "Source/DialogueMemoryTable/Private/DialogueMemoryTableSubsystem.cpp",
        ROOT / "Source/DialogueMemoryTable/Private/AlchemyGameplaySubsystem.cpp",
        ROOT / "Source/DialogueMemoryTable/Private/DialogueAIServiceSubsystem.cpp",
        ROOT / "Source/DialogueMemoryTable/Private/DailyMushroomSubsystem.cpp",
        ROOT / "Source/DialogueMemoryTable/Private/DialogueMemoryTableModule.cpp",
    ]
    for source_file in source_files:
        require(source_file.is_file(), f"missing source file: {source_file}")
        check_balanced_cpp(source_file)

    combined = "\n".join(
        source_file.read_text(encoding="utf-8")
        for source_file in source_files
    )
    require('TEXT("""' not in combined, "mangled quote literal detected")
    for symbol in (
        "ApplyMemoryPatchJson",
        "SetAuthoritativeNpcProgress",
        "UpsertAuthoritativeWorldState",
        "ExportLedgerToCsv",
        "HasOnlyFields",
        "EvaluatePersonaTrigger",
        "AdvancePersonaFromPlayerText",
        "BuildPersonaSystemPrompt",
        "EvaluateAlchemySequence",
        "BuildWorldConsequencePrompt",
        "RequestChatCompletion",
        "RequestNpcDialogue",
        "RequestWorldConsequence",
        "AdvanceDay",
        "GetDailyMushrooms",
        "GetMushroomValue",
    ):
        require(symbol in combined, f"missing implementation symbol: {symbol}")

    require("MakeBlacksmithProfile" in combined, "missing blacksmith persona")
    require("MakeOperatorProfile" in combined, "missing operator persona")
    require("MakeNovelistProfile" in combined, "missing novelist persona")
    require(combined.count("FamiliarityLevel = 3") >= 3, "missing third persona layers")

    print("DialogueMemoryTable package checks passed.")
    print("Note: an Unreal Build Tool compile is still required in the target project.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, KeyError, TypeError, ValueError) as error:
        print(f"Validation failed: {error}", file=sys.stderr)
        raise SystemExit(1)
