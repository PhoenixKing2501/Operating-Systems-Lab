#!/usr/bin/python3
import sys
in_file = open(sys.argv[1])
word_count = 0

content = in_file.read().replace('=', ' = ').replace('|', ' | ').replace(';', ' ; ').replace(',', ' , ').replace('<', ' < ').replace('>', ' > ').replace('(', ' ( ').replace(')', ' ) ').replace('[', ' [ ').replace(']', ' ] ').replace('{', ' { ').replace('}', ' } ')
word_count += len(content.split())
print(f'Word count: {word_count}')