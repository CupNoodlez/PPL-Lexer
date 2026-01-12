start

#ITERATIVE
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

end