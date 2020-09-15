Author | Date
--- | ---
MountainX | 2020-01-24T00:25:20Z

Experimental push on scalar is now forbidden at /usr/local/bin/indent2tree line 43, near "};"

Relevant?
https://stackoverflow.com/a/41192741
---

Author | Date
--- | ---
ikegami | 2020-01-24T00:25:20Z

    push $ForkPoint->{subtree}, ...

should be

    push @{ $ForkPoint->{subtree} }, ...

---

Author | Date
--- | ---
MountainX | 2020-01-24T00:25:20Z

This resolves it for me. Thank you.

---

Author | Date
--- | ---
MountainX | 2020-01-24T00:25:20Z

Thank you. It is working for me now.
