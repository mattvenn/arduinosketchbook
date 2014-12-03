import Tkinter
import time

tk = Tkinter.Tk()


p = Tkinter.DoubleVar()
i = Tkinter.DoubleVar()
d = Tkinter.DoubleVar()

#scales
p_s =   Tkinter.Scale(label='p',from_=0, to=1000, orient=Tkinter.VERTICAL,variable=p)
i_s =   Tkinter.Scale(label='i',from_=0, to=1000, orient=Tkinter.VERTICAL,variable=i)
d_s =   Tkinter.Scale(label='d',from_=0, to=10, orient=Tkinter.VERTICAL,variable=d)

p_s.pack()
i_s.pack()
d_s.pack()

interval = 100
#go!
import sys
def printer(tk):
    #print("%f,%f,%f" % (p.get(),i.get(),d.get()))
    print("%f %f" % (p.get(),i.get())) #,d.get()))
    sys.stdout.flush()
    tk.after(interval,printer,tk)

tk.after(interval,printer,tk)
tk.mainloop()
