## Why are clock and timing important in MCUs.

- The clock is what paces all the execution of code inside the processor. 
- The clock basically tells all parts of the CPU to march in lockstep. 
- If the CPU is an orchestra, the clock is the conductor. This way, each part 
of the CPU knows when another part is about to do something. 
- This makes communication inside the CPU far simpler and more reliable. 
- Time and clock also plays an important to how a normal society function. 
For example, people usually need to know when to do what. What's the deadline.

### ELI5

- If you add a faster or slower clock, it will just run faster or slower to a 
point.
- If you put on a clock that's too fast, things will not work in unpredictable 
ways.
- Digital logic relies on things changing states in various ways. The physical 
processes by which this happen take different times. Here's an eli5 (or 10)
- 8 people at a table, some sitting, some standing, are brought envelopes. 
The envelopes contain instructions. 
- When the clock dings, they are to open their envelopes and follow the 
instructions. 
- Depending on the instruction and the kind of person, it will take them various 
amounts of time to complete their instruction.
- Depending on the result they get, they remain sitting/standing or they stand/sit.
- When the clock dings again, you count the number of people standing, write it 
down on an envelope, and take it to the next room, where you will give it to one 
of 8 people around a table....
- Some of the people might be a bit older and take a bit longer. As long as they 
get their answer and are able to stand/sit on time, everything is OK.
- But if your clock is too fast for that person or if they're just not as fast as 
they used to be, instead of going to the next room with a 4, you're going to go in 
with a 5 or 3. That's going to mess up whatever the answer from that next room was 
supposed to be. 
- This is why aging, over clocked or otherwise damaged logic units just return the 
wrong results and why CPUs don't actually slow down when their components get 
slower, they start returning the wrong answers.
