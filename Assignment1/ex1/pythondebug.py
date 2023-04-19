
#count words testepython.txt

import string
def count_words(filename):
    count = {"a":0, "e":0, "i":0, "o":0, "u":0}
    for line in open(filename):
        line = line.split()
        for word in line:
            print(word, end=" ")
            if "a" in word or "A" in word:
                count["a"] += 1
            if "e" in word or "E" in word:
                count["e"] += 1
            if "i" in word or "I" in word:
                count["i"] += 1
            if "o" in word or "O" in word:
                count["o"] += 1
            if "u" in word or "U" in word:
                count["u"] += 1
    print()
    print(count)


count_words('testepython.txt')