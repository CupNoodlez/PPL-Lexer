template can_enter_forest(character):
    if character.trust >= 50:
        return True
    else:
          return False

if can_enter_forest(Elio):
    show scene: "Forest"
