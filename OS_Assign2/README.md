## OS Project 2: Forking and Exec()
Joseph Hong

### Description
This is a program that creates a hierarchy of processes using fork() and uses the exec*() system call(s) to have nodes accomplish potentially diverse tasks. (Copied from the prompt.)

### Instructions for Running
1) on terminal, in directory, call "make" to create the executable, *myhie*
1.5) call 'make clean' to remove all the excess .o files
2) call './myhie -i *InputFile* -r -k *NumOfWorkers* -a *AttributeNumber* -t *Type* -o *Order* -s *OutputFile*' where:
    - *InputFile* the file name containing the data- records

    - *NumOfWorkers* is the number of sorters to be spawned

    - '-r' is the flag that instructs the program to have workers work on “random ranges” or not-equally-sized batches of data-records

    - *AttributeNumber* is a valid numeric-id that designates the field on which sorting is to be carried out
    
    - *Order* is either ascending (a) or descending (d)
    
    - *OutFile* is the file in which the outcome of myhie could be saved.

### Recommendations and Tips; Lessons Learned
So, some stuff I learned.
First of all, the last file with 1 million entries kills my computer. I've gone up to 100,000 (took like 5 full minutes on my computer which runs on M1) entries, but beyond that might be a little whack.
Also, too many workers will break the program because of the number of print statements that the merger needs to send to the root. For example, I can pull of 100,000 entries on my computer with 10 sorters, but not with 1000. Because of the number of things that each process needs to go through (I decided on some pretty slow algorithms and also added a *general* sorting method for the merger) is so great, the program can't help but, well, *die*.

### The Structure and How it Works
Okay, so let's talk about the structure. I had a terrible, terrible time getting this to work, so please tolerate my moments of passive aggressiveness towards the nodes.

#### The Root (of All The Nightmares)
First off, we have the root. This was perhaps the easiest (until the end, I'll get to that) node to implement because all I needed to make was an arguments checker and fork() the coord. There isn't really anything special about this node, except maybe the communication on SIGUSR2, which allows the merger node to send a list of all of the sorter times and its own time. I did this by creating a separate handler for SIGUSR2 (this was because there are times when signals overlap with each other and I couldn't have it missing the root).

#### The Coord(inator of Despair)
Then we have the coord. This was one of the iffier things to implement, right after the root. The coord node takes the arguments that are passed to it from the root and creates sorters, and then a merger node out of them. It also has a dynamic file line counter, which was something I decided to do only on the coord so that I didn't have to keep doing it later (and didn't have to rely on the file name). Anyway, this makes the pipes (FIFOs; this was an absolute nightmare but that'll come later) for each of the sorters and then for the merger. It takes the merger's FIFO and sends the data on the sorter PIDS and their numLines (the ranges they're sorting).

#### The Sorter
The sorter node, well, sorts. There are two algorithms I used: bubble sort and selection sort. I explained how it works in the code, but the concepts are very simple. Both involve swapping, but where bubble sort doesn't have a designated "sorted section", selection sort does. There are different supporting functions that I used to implement this, some of which are from outside sources (GeeksandGeeks; link in the code), but I'm particularly happy about the compare() function.

#### Compare.
Basically, what this does is it returns a value that is dependent on isDescending, which is the argument that changes the display format for the entries. The explanation is in the code, but I thought that it was really cool because I didn't have to make a separate "if descending do this, else do this". All I needed to make was "compare".

#### Sending Data: The Nightmare Begins
Now. Sending the data. This was the beginning of the nightmare. My code is based around strings, and you can't actually send strings through pipes (at least from what I've seen) because they're technically *pointers*. So, this meant I needed to convert everything from strings to character arrays, then back to strings again. At first, I sent all the data in a single line, but then I realized that once the files got bigger (or the ranges were increased) the pipe buffers were actually *too small*. This led to hours and hours trying to implement a line-by-line sending method.
I got it to work eventually (the way that I say this is so nonchalant but you should have watched me screaming at my computer to work), using a for loop with the number of lines. This was actually why I send the numLines of all the sorters to the merger from the coord. I didn't have this at first and was trying to read the lines as they came (if the buffer != 0), but that didn't work (AT ALL). I dabbled in sigaction() trying to get the PID of the merger from the sorters by sending a signal, but that went terribly so I gave up. But hey, I got it to work in the end... 

#### The Merger: Molding All the Nightmares into One Big Blob
And then the merger. There isn't much to say about this one, since everything has been basically summarized above. Nightmare after nightmare trying to get the communication between the processes to work, a lot of screaming, and a few manly tears, but it's working. Something to note here is that the merger receives both the sorted lines *and* the time report from the sorters. This was because I had an issue where signals (SIGUSR1) from the sorters would overlap when I initially had the root open up a pipe every time a signal was sent, so some time reports were skipped. This way, I could store all the time reports and send them to the root at the end so that none were missed. The downside of this is that it's a lot of printing and this can kill the program when there are too many jobs. But it's still really cool, at least to me.
Another thing is that the merger actually sort of sorts (hehe) the overall list of entries once it receives all of them from the sorters. This is done by tracking all of the starting indicies of the sorters and picking the min/max value (depending on isDescending) then putting that into a new array. This is also why the program gets really slow when there are a lot of entries.

#### Overall
This was fun, but really, *really* frustrating at times. Mostly frustrating with FIFO. Almost entirely FIFO. It's cool, but hard to learn. The same goes with signals. I wish we had learned more about sigaction(), but then again it looks really complicated.
