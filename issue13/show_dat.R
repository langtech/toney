tab = read.table("test.dat")
plot(tab$V3, pch=20, col="green", cex=2.0)
points(tab$V2, pch=20, col='red', cex=1.0)
points(tab$V1, pch=20, col='black', cex=0.1)




tab2 = read.table("test.smooth")
lines(tab2$V1, col='blue', lty=3, lwd=1)

library(signal)
f = butter(3, 0.1)
x = filtfilt(f$b, f$a, tab$V2)
lines(x, col='magenta', lty=4)
