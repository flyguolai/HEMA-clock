# AGENTS.md

## What this repo is

Documentation-only project about repurposing Hema (盒马) ESL (Electronic Shelf Label) devices into ink screen clocks. No source code, no build system, no tests, no CI.

All content is in Chinese (Simplified).

## Repo structure

- `docs/` — flashing tutorials, firmware options, hardware info, development guide
- `references/` — pinout diagrams, community links, external resources
- `README.md` — project overview and quick start

## Key constraints

- **No executable code**: there is nothing to build, lint, or test. Edits are markdown-only.
- **Language**: all user-facing content is Simplified Chinese. Preserve this in edits.
- **Hardware-specific**: content targets Renesas DA14585 BLE SoC + SSD1680-driven 2.13" e-paper displays. Don't generalize hardware details.
- **External links**: many links point to myetoys.com, eefocus.com, and Renesas. Verify links before adding new ones.

## Editing guidance

- When adding firmware entries, follow the existing table format in `docs/firmware-options.md`.
- Pinout references go in `references/pinout.md`.
- Community/external resources go in `references/links.md` or `references/community.md`.
- Development workflows and code examples go in `docs/development.md`.
