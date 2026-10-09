"""Appends this conversation's new messages to dev_chat.log in the project's main folder.

Reads the Claude Code transcript (user prompts and the assistant's replies, not tool output) and writes only what the log
does not have yet; the ids it has already written are kept in .dev_chat_state.json next to the log.

    python Tools/dev_chat_sync.py [transcript.jsonl]
"""
import json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LOG = os.path.join(ROOT, "dev_chat.log")
STATE = os.path.join(ROOT, ".dev_chat_state.json")
DEFAULT = r"C:\Users\User\.claude\projects\K--Epic2-PuzzleMagic\676dd3db-e75e-4ed7-a079-1880a1623fa7.jsonl"
NOISE = re.compile(r"<(system-reminder|local-command-caveat|task-notification|command-message|pasted_content[^>]*)>.*?</\1>", re.S)
TAGS = re.compile(r"</?(command-name|command-args|local-command-stdout)>")


def text_of(content, role):
    if isinstance(content, str):
        parts = [content]
    else:
        parts = [b.get("text", "") for b in content if isinstance(b, dict) and b.get("type") == "text"]
    out = NOISE.sub("", "\n".join(parts))
    out = TAGS.sub("", out).strip()
    return out


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else DEFAULT
    done = set()
    if os.path.exists(STATE):
        done = set(json.load(open(STATE, encoding="utf-8")))
    new = []
    for line in open(src, encoding="utf-8"):
        try:
            d = json.loads(line)
        except ValueError:
            continue
        if d.get("type") not in ("user", "assistant") or d.get("isSidechain") or d.get("isMeta"):
            continue
        uid = d.get("uuid")
        if not uid or uid in done:
            continue
        done.add(uid)
        text = text_of(d.get("message", {}).get("content", ""), d["type"])
        if not text:
            continue
        who = "YOU" if d["type"] == "user" else "CLAUDE"
        new.append("[%s] %s\n%s\n" % ((d.get("timestamp") or "")[:19].replace("T", " "), who, text))
    if new:
        with open(LOG, "a", encoding="utf-8", newline="\n") as f:
            if f.tell() == 0:
                f.write("PuzzleMagic dev chat log (YOU = the designer, CLAUDE = the assistant; tool output left out)\n\n")
            f.write("\n".join(new) + "\n")
    json.dump(sorted(done), open(STATE, "w", encoding="utf-8"))
    print("appended %d messages to %s" % (len(new), LOG))


main()
