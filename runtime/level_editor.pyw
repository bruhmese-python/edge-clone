import tkinter as tk
from tkinter import messagebox
import tkinter.font as tkFont


class App:
    squares = dict()
    sq_widgets = dict()
    lastclicked = str()
    perline = 0

    def save(self, event):
        l = list(self.squares.values())
        f = open('lvl1.lvl', 'w')

        x = 0
        while x in range(l.__len__()):
            if x % self.perline == 0:
                f.write(''.join(map(str, l[x:x + self.perline])) + '\n')
            x += 1
        messagebox.showinfo(message="Done")

    def __init__(self, root):
        self.root = root

        self.root.title("Level-Editor")
        width = 280
        height = 280
        screenwidth = self.root.winfo_screenwidth()
        screenheight = self.root.winfo_screenheight()
        alignstr = '%dx%d+%d+%d' % (width, height,
                                    (screenwidth - width) / 2, (screenheight - height) / 2)
        canvas_height = height
        canvas_width = width

        x = canvas_width
        y = canvas_height

        i = 2
        while i in range(0, x - 10):
            j = 2
            while j in range(0, y - 10):

                def colorblock(event):
                    if self.lastclicked:
                        key2 = event.widget.cget('text').split(",")
                        key1 = self.lastclicked.split(",")
                        _i = int(key1[0])
                        while _i in range(int(key1[0]), int(key2[0])):
                            _j = int(key1[0])
                            while _j in range(int(key1[1]), int(key2[1])):
                                _key = str(_i) + "," + str(_j)
                                self.squares[_key] = 1
                                self.sq_widgets[_key].config(bg="yellow")
                                _j += 11
                            _i += 11

                def color(event):
                    key = event.widget.cget('text')
                    if self.squares[key] == 0:
                        self.squares[key] = 1
                        event.widget.config(bg="yellow")
                    elif self.squares[key] == 1:
                        self.squares[key] = 2
                        event.widget.config(bg="blue")
                    elif self.squares[key] == 2:
                        self.squares[key] = 3
                        event.widget.config(bg="red")
                    elif self.squares[key] == 3:
                        self.squares[key] = 4
                        event.widget.config(bg="green")
                    elif self.squares[key] == 4:
                        self.squares[key] = 0
                        event.widget.config(bg="white")
                    self.lastclicked = key

                key = str(i) + "," + str(j)
                rect = tk.Button(self.root)

                rect.bind('<Button-1>', lambda event: color(event))
                rect.bind('<Button-3>', colorblock)

                rect.place(x=i, y=j, width=10, height=10)
                ft = tkFont.Font(family='Times', size=10)
                rect["bg"] = "white"
                rect["fg"] = "white"
                rect["text"] = key
                rect["font"] = ft

                self.squares[key] = 0
                self.sq_widgets[key] = rect
                j += 11
            self.perline += 1
            i += 11
        self.root.bind('<Control-s>', self.save)
        self.root.geometry(alignstr)
        self.root.resizable(width=False, height=False)
        self.temp = list(self.squares)


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()
