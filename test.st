start 

# INPUT 
ask Elio "What is your name?" as name


# ITERATIVE
repeat until health <= 0:
    damage = damage + 5
    health = health - damage
    show narrator: health

# OUTPUT
narrate: "The castle gates open slowly"
narrate hero.name: "The hero approaches the castle"
dialogue hero: "The hero approaches the castle"
show: "The hero approaches the castle"
show andrea: "Yehey!"
show Ocean: Beach_part
dialogue hero: "We must hurry!" + guard1

# ASSIGNMENT STATEMENT

# basic assign
andrea = x + y * g

# attribute assign
player.hp = 100

#string literal assign
denmar = "goat"

#float literal
ivan = 3.5

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
if Elio.mood is "calm":
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
repeat 3 times:
    show narrator: "Knock knock knock!"

# DECLARATION
character Hero, Wingman, Villain

end