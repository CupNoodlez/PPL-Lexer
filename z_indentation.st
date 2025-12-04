<<<<<<< HEAD
ask Guard1  "what are u doing" as Wingman

dialogue Guard1:
	ask Elio “What are u doing” as answer
narrate:
	ask Elio “How old are you?” as age
	Elio.age = age

ask character
=======
template  can_enter_forest(character):
    if character.trust      >=       50:
        return True
    else:
          return False

if can_enter_forest(Elio):
        show scene: "Forest"
>>>>>>> origin/dev
