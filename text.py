#!/usr/bin/env python3
import tkinter

colours = {
        "bgcolour" : "black", # text area background colour
        "fgcolour" : "white", # text area 
        "selcolour" : "yellow", # colour of newest selection
        "selocolour" : "red", # colour of older selectons
        "curscolour" : "green", # colour of cursor
        }

def quit(event):
  root.destroy()
  print( "bye now\n" )
  exit()

def selected(event):
    print("UMMM\n")

# for modifier docu see http://search.cpan.org/~ni-s/Tk-804.027/pod/bind.pod#MODIFIERS
bindings = {
        "<<Selection>>" : selected, # calls selected whenever the selection changes
        "<Control-q>" : quit,
        }

root = tkinter.Tk(className="ark")
text = tkinter.Text(root, background=colours["bgcolour"], foreground=colours["fgcolour"], insertbackground=colours["curscolour"])
text.pack(fill=tkinter.BOTH, expand=1)

text.tag_config("sel", background=colours["selcolour"])
text.tag_config("selo", background=colours["selocolour"])

for key,value in bindings.items():
    if key[0] == "<" and key[1] == "<":
        # special text event
        text.bind(key, value)
    else:
        root.bind(key, value)

root.mainloop()
