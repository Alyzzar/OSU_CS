import random


#Create lists for variable generation
strings = [None, None, None]
alph = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z']

#Loop for each of the three strings
for i in range(0,3):
    #Loop for each of the 10 chars, and assign random chars
    for j in range(0, 10):
        strings[i] = strings[i] + random.choice(chars)
    strings[i] = strings[i] + "\n"
        
#Create/Overwrite 3 files (any names)
#Save the vars to the files
f= open("chanbe.output1.txt","w+")
f.write(strings[0])
f.close()

f= open("chanbe.output2.txt","w+")
f.write(strings[1])
f.close()

f= open("chanbe.output3.txt","w+")
f.write(strings[2])
f.close()

#Print the contents of the three variables
print(strings[0] + strings[1] + strings[2])

#Set vars of two random integers and their value
num1 = random.randint(0, 42)
num2 = random.randint(0, 42)
num3 = num1 + num2

#Print the values
print(num1 + '\n' + num2 + '\n' + num3)
