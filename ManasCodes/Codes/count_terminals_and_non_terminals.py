traversed = list()
traversed.append("->")
terminals = 0
non_terminals = 0
for s in open("Grammar.txt").read().split():
    if s not in traversed:
        if s[0] == "<":
            non_terminals += 1
        else:
            terminals += 1
        traversed.append(s)
print("terminals =", terminals)
print("non_terminals =", non_terminals)
