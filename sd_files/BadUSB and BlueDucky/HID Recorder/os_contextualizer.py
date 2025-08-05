#!/usr/bin/env python3
"""
DuckScript OS Profile Contextualizer
Adapts DuckScript commands based on target OS profiles
Part of the Bruce HID Recorder validation pipeline
"""

import json
import argparse
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, asdict


@dataclass
class OSProfile:
    """Operating System profile for DuckScript adaptation"""

    name: str
    run_command: str  # How to open run dialog
    terminal_command: str  # How to open terminal
    file_manager: str  # How to open file manager
    spotlight_search: str  # How to open search
    task_manager: str  # How to open task manager
    alt_tab: str  # How to switch windows
    copy_shortcut: str  # Copy command
    paste_shortcut: str  # Paste command
    select_all: str  # Select all command


class DuckScriptContextualizer:
    def __init__(self):
        # Predefined OS profiles
        self.profiles = {
            "windows": OSProfile(
                name="Windows",
                run_command="GUI r",
                terminal_command="GUI r\nDELAY 500\nSTRING cmd\nENTER",
                file_manager="GUI e",
                spotlight_search="GUI s",
                task_manager="CTRL SHIFT ESC",
                alt_tab="ALT TAB",
                copy_shortcut="CTRL c",
                paste_shortcut="CTRL v",
                select_all="CTRL a",
            ),
            "macos": OSProfile(
                name="macOS",
                run_command="GUI SPACE",
                terminal_command="GUI SPACE\nDELAY 500\nSTRING terminal\nENTER",
                file_manager="GUI SPACE\nDELAY 500\nSTRING finder\nENTER",
                spotlight_search="GUI SPACE",
                task_manager="GUI SPACE\nDELAY 500\nSTRING activity monitor\nENTER",
                alt_tab="GUI TAB",
                copy_shortcut="GUI c",
                paste_shortcut="GUI v",
                select_all="GUI a",
            ),
            "linux": OSProfile(
                name="Linux",
                run_command="ALT F2",
                terminal_command="CTRL ALT t",
                file_manager="GUI e",
                spotlight_search="GUI",
                task_manager="CTRL SHIFT ESC",
                alt_tab="ALT TAB",
                copy_shortcut="CTRL c",
                paste_shortcut="CTRL v",
                select_all="CTRL a",
            ),
        }

        # Contextual command patterns to replace
        self.context_patterns = {
            "OPEN_RUN": "run_command",
            "OPEN_TERMINAL": "terminal_command",
            "OPEN_FILEMANAGER": "file_manager",
            "OPEN_SEARCH": "spotlight_search",
            "OPEN_TASKMANAGER": "task_manager",
            "SWITCH_WINDOW": "alt_tab",
            "COPY": "copy_shortcut",
            "PASTE": "paste_shortcut",
            "SELECT_ALL": "select_all",
        }

    def save_profiles(self, file_path: Path):
        """Save current profiles to JSON file"""
        profiles_dict = {
            name: asdict(profile) for name, profile in self.profiles.items()
        }
        with open(file_path, "w") as f:
            json.dump(profiles_dict, f, indent=2)
        print(f"✅ Profiles saved to {file_path}")

    def load_profiles(self, file_path: Path):
        """Load profiles from JSON file"""
        if not file_path.exists():
            print(f"❌ Profile file not found: {file_path}")
            return False

        try:
            with open(file_path, "r") as f:
                profiles_dict = json.load(f)

            self.profiles = {}
            for name, profile_data in profiles_dict.items():
                self.profiles[name] = OSProfile(**profile_data)

            print(f"✅ Loaded {len(self.profiles)} profiles from {file_path}")
            return True
        except Exception as e:
            print(f"❌ Error loading profiles: {e}")
            return False

    def contextualize_script(
        self, input_script: List[str], target_os: str
    ) -> List[str]:
        """Apply OS-specific context to DuckScript commands"""
        if target_os not in self.profiles:
            print(f"❌ Unknown OS profile: {target_os}")
            print(f"Available profiles: {list(self.profiles.keys())}")
            return input_script

        profile = self.profiles[target_os]
        output_script = []

        for line in input_script:
            line = line.strip()
            if not line or line.startswith("//") or line.startswith("#"):
                output_script.append(line)
                continue

            # Check for contextual patterns
            contextualized = False
            for pattern, profile_attr in self.context_patterns.items():
                if line.upper().startswith(pattern):
                    # Replace with OS-specific command
                    replacement = getattr(profile, profile_attr)
                    output_script.extend(replacement.split("\n"))
                    contextualized = True
                    break

            if not contextualized:
                # Apply simple substitutions for common shortcuts
                if line.upper() == "COPY":
                    output_script.append(profile.copy_shortcut)
                elif line.upper() == "PASTE":
                    output_script.append(profile.paste_shortcut)
                elif line.upper() == "SELECT_ALL":
                    output_script.append(profile.select_all)
                else:
                    output_script.append(line)

        return output_script

    def process_file(self, input_file: Path, output_file: Path, target_os: str) -> bool:
        """Process a DuckScript file and apply OS context"""
        if not input_file.exists():
            print(f"❌ Input file not found: {input_file}")
            return False

        try:
            with open(input_file, "r", encoding="utf-8") as f:
                lines = f.readlines()
        except Exception as e:
            print(f"❌ Error reading input file: {e}")
            return False

        # Strip newlines for processing
        input_lines = [line.rstrip("\n\r") for line in lines]

        print(f"🔄 Contextualizing {len(input_lines)} lines for {target_os}...")

        # Apply contextualization
        output_lines = self.contextualize_script(input_lines, target_os)

        # Add header comment
        header = [
            f"// DuckScript contextualized for {self.profiles[target_os].name}",
            f"// Generated from: {input_file.name}",
            f"// Target OS: {target_os}",
            f"// Commands: {len(output_lines)}",
            "//",
            "",
        ]

        final_output = header + output_lines

        try:
            with open(output_file, "w", encoding="utf-8") as f:
                for line in final_output:
                    f.write(line + "\n")
        except Exception as e:
            print(f"❌ Error writing output file: {e}")
            return False

        print(f"✅ Contextualized script saved to {output_file}")
        print(f"📊 Input: {len(input_lines)} lines → Output: {len(output_lines)} lines")

        return True

    def show_profiles(self):
        """Display available profiles"""
        print("🖥️  Available OS Profiles:")
        print("=" * 50)

        for name, profile in self.profiles.items():
            print(f"\n{profile.name} ({name}):")
            print(f"  Run Dialog:    {profile.run_command}")
            print(f"  Terminal:      {profile.terminal_command[:30]}...")
            print(f"  File Manager:  {profile.file_manager}")
            print(f"  Search:        {profile.spotlight_search}")
            print(f"  Task Manager:  {profile.task_manager}")

    def show_patterns(self):
        """Display available contextual patterns"""
        print("🎯 Contextual Patterns:")
        print("=" * 50)
        print("Use these patterns in your DuckScript for OS-agnostic commands:")
        print()

        for pattern, description in self.context_patterns.items():
            print(f"  {pattern:20} → {description}")

        print("\nExample DuckScript with contextual patterns:")
        print("  OPEN_RUN")
        print("  STRING notepad")
        print("  ENTER")
        print("  DELAY 1000")
        print("  STRING Hello World!")
        print("  SELECT_ALL")
        print("  COPY")


def main():
    parser = argparse.ArgumentParser(description="DuckScript OS Profile Contextualizer")
    parser.add_argument("input_file", nargs="?", help="Input DuckScript file")
    parser.add_argument(
        "-o", "--output", help="Output file (default: input_file_OS.txt)"
    )
    parser.add_argument(
        "-t",
        "--target",
        choices=["windows", "macos", "linux"],
        help="Target OS profile",
    )
    parser.add_argument(
        "--show-profiles", action="store_true", help="Show available OS profiles"
    )
    parser.add_argument(
        "--show-patterns",
        action="store_true",
        help="Show available contextual patterns",
    )
    parser.add_argument("--save-profiles", help="Save current profiles to JSON file")
    parser.add_argument("--load-profiles", help="Load profiles from JSON file")

    args = parser.parse_args()

    contextualizer = DuckScriptContextualizer()

    # Load custom profiles if specified
    if args.load_profiles:
        contextualizer.load_profiles(Path(args.load_profiles))

    # Show information commands
    if args.show_profiles:
        contextualizer.show_profiles()
        return

    if args.show_patterns:
        contextualizer.show_patterns()
        return

    # Save profiles if requested
    if args.save_profiles:
        contextualizer.save_profiles(Path(args.save_profiles))
        return

    # Process file
    if args.input_file and args.target:
        input_file = Path(args.input_file)

        if args.output:
            output_file = Path(args.output)
        else:
            # Generate output filename
            stem = input_file.stem
            suffix = input_file.suffix
            output_file = input_file.parent / f"{stem}_{args.target}{suffix}"

        contextualizer.process_file(input_file, output_file, args.target)
    else:
        print("Usage examples:")
        print("  python os_contextualizer.py --show-profiles")
        print("  python os_contextualizer.py --show-patterns")
        print("  python os_contextualizer.py script.txt -t windows")
        print("  python os_contextualizer.py script.txt -t macos -o mac_script.txt")


if __name__ == "__main__":
    main()
