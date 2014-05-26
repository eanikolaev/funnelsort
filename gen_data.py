import random
FILENAME = "data"
MAX = 100500


if __name__ == '__main__':
    f = open(FILENAME, 'w')
    for i in range(MAX):
        f.write(str(random.randint(0,MAX))) 

    f.close()
