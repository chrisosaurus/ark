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

root.bind("<Control-q>", quit)
root.mainloop()
