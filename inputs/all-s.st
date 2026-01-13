start 

# INPUT 
ask Elio "What is your name?" as name

# DECLARATION
template Slap(x, y):
    narrate: "A loud slap echoes."
    dialogue x: "Hey! Why did you slap me?"
    dialogue y: "Because you ate my lunch!"
    mood = "angry"



# ITERATIVE
repeat until health  <= 0:
    damage = damage + 5
    health = health - damage
    show narrator: health

# OUTPUT
narrate: "The castle gates open slowly"
narrate hero.name: "The hero approaches the castle"
dialogue hero: "The hero approaches the castle"
show: "The hero approaches the castle"
show andrea: "Yehey!"
show Ocean: beach
dialogue hero: "We must hurry!" + guard1

# DECLARATION
character andrea

# ASSIGNMENT STATEMENT

# basic assign
andrea = x + y * g

# attribute assign
player.hp = 100

#string literal assign
denmar = "goat"

#float literal
ivan = 3.0

#attribute with assignment
player.score += 5
enemy.hp -= 20
level.up *= 2
distance /= 3
counter %= 10

#simple arithmetic
x = age + yourAge

#nested arithmetic
result = x + b * c - d

#nested with parentheses
total = (price * qty) - discount

#unary 
value = - x

#multiple unary
x = --- y

#logical compound
flag = x > 10 or y < 5
ready = (hp > 0 and stamina > 10) or is_awake

# bool with arithmetic
active = value > 10 and count % 2 == 0

#ITERATIVE
for hero in party:
  show hero: "Ready for battle!"
  dialogue:
    "Let's go!"

# CONDITIONAL STATEMENT

# if
if "calm":
    scene blue: 
        describe, "andrea"

# if else
if Elio.mood is "calm":
    scene blue: 
        describe, "andrea"
else: 
    scene denmar: 
        describe, "denmar"

# elif 
if temperature > 30:
    scene blue: 
        describe, "andrea"
elif temperature > 40 and temperature <= 50: 
    scene denmar: 
        describe, "denmar"
else:
    scene reymel: 
        describe, "reymel"

# ITERATIVE
for hero in party:
  show hero: "Ready for battle!"
  dialogue:
    "Let's go!"
  for hero in party:
    show hero: "Ready for battle!"
    dialogue:
      "Let's go!"
      
    repeat 3 times:
      show narrator: "Knock knock knock!"

# DECLARATION
character Hero, Wingman, Villain

end