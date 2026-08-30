# Antelope Core Developer Skill

A Cursor Agent Skill that applies the engineering mindset used on AntelopeIO/spring and leap: correctness first, sanitizer-clean C++, reviewable changes, and long-running node robustness.

## Install

**Personal (all projects):**

```bash
git clone https://github.com/<you>/antelope-core-dev-skill.git ~/.cursor/skills/antelope-core-dev
```

**Project (this repo only):**

```bash
mkdir -p .cursor/skills
git clone https://github.com/<you>/antelope-core-dev-skill.git .cursor/skills/antelope-core-dev
```

Or copy `SKILL.md` into `~/.cursor/skills/antelope-core-dev/SKILL.md` or `.cursor/skills/antelope-core-dev/SKILL.md`.

## When it applies

The skill is meant to load for C++ systems work on long-running nodes, blockchain infrastructure, concurrent timers, sanitizers, packaging, ABI correctness, protocol features, or high-stakes state-machine changes.

## License

GNU General Public License v3.0. See [LICENSE](LICENSE).
