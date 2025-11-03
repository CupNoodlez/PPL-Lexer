character X

character X:
    role=”Protagonist

scene Town:
    [“Town_Gate”, scene description],
    [“Town_House”, scene description],
    [“Town_Castle”, scene description]

Show scene.Town: “Town_Gate”

template greet(character):
    dialogue character: "Hello!"
    emotion character "happy"

greet(X)

dialogue X: 
    “Hello, World!”

narrate: “Miranda_Rights.txt”

choice as level:
    [“A”, <choice description>],
    [“B”, <choice description>],
    [“C”, <choice description>]

option “Accept” then X.power += 100
option “Reject” then X.power -= 5

ask “Is it a yes or no?”  as answer

if X.power is 100:
    Transform(X)

if X.power is 100:
    Transform(X)
elif X.power < 100:
    Holdback(X)

if X.power is 100:
    Transform(X)
elif X.power < 100:
    Holdback(X)
else:
    Run(X)

repeat 3 times:
    Run(X)

repeat until X.energy == 0:
     Run(X)

for i in [1, 2, 3, 4, 5]:
    jump(X)
    
show scene.Town: “Town_Hall”
start:
    <code>
start:
    <code>
end
X.emotion = ‘happy’

X.emotion becomes ‘angry’
if X.money > 1000 and X.status is “Peasant”:
if not X.status is “Rich” then show scene.Garbage : “Scavenging”
if X.emotion is “happy” or X.emotion is “grateful” then show scene.Party : “Dancing”


isHappy = True
isRich = False
template can_enter_forest(character):
    if character.trust >= 50:
        return True
    else:
        return False

if can_enter_forest(Elio):
    show scene: "Forest"

repeat until X.energy == 0:
     Run(X)
repeat 3 times:
     Run(X)
for scene in scene.RedemptionArc:
    show scene
ask "Do you want to go left or right?" as choice

if choice != "left" and choice != "right":
    error "Invalid choice! Must be 'left' or 'right'."

character X:
    fixed role = “Protagonist”
for scene in show scene.Adventure:
    if scene == "FinalBattle":
        break
    show scene

repeat until X.trust < 100:
    X.trust += 10
    if X.trust == 50:
        continue
    dialogue X: "Do you trust me yet?"

if X.mood == "neutral":
    pass
else:
    dialogue X: "I feel something."
if X.mood is "happy":
    dialogue X: "I'm feeling great!"


show character with mood = "joyful"
show scene "TownSquare" with time = "sunset"

