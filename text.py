#!/usr/bin/env python3
import tkinter

root = tkinter.Tk(className="ark")
text = tkinter.Text(root, background="black", foreground="white", insertbackground="white")
text.pack(fill=tkinter.BOTH, expand=1)

text.tag_config("sel", background="yellow")
text.tag_config("selo", background="red")

def quit(event):
  root.destroy()
  print( "bye now\n" )
  exit()

# for modifier docu see http://search.cpan.org/~ni-s/Tk-804.027/pod/bind.pod#MODIFIERS
root.bind("<Control-q>", quit)

def selected(event):
    print("UMMM\n")

# calls selected whenever the selection changes
text.bind("<<Selection>>", selected)
root.mainloop()
