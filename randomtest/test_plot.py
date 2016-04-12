import matplotlib.pyplot as pl
import pickle
with open('data.pk') as fh:
    data = pickle.load(fh)

results = []
x = range(10)
for i in x:
    results.append(data.count(i))
fig = pl.figure()
ax = pl.subplot(111)
ax.bar(x, results)
#pl.show()
pl.savefig("random.png")
