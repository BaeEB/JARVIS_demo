import os
import subprocess


def write_branch_list():
    print(os.system("git branch -r | grep -Eo '/.*auto-patch.*' > branch_list"))

def refine_branch_list():
    branch_line = []
    new_branch_line = []
    with open('branch_list', 'r') as f:
        branch_line = f.readlines()
    for line in branch_line:
        new_branch_line.append(line.replace('/', ''))
        os.system(f"git push origin -d {line.replace('/', '')}")
    print(new_branch_line)
    

write_branch_list()
refine_branch_list()