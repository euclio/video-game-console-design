# E190U Lab 3: <small>Gameplay Testing</small>
<address>Andy Russell</address>

## Introduction

In this lab, I worked with Bryan to devise a testing strategy for our
controllers that we designed in labs 1 and 2. We modified the open-source game
[Lugaru][lugaru-homepage] to log interesting game statistics. We then examined
the data obtained from a number of tests to see if it offered any insight into our controllers' performance.

Bryan's writeup for this lab is located [here][bryans-writeup].

## Design Methodology

Bryan and I decided on modifying Lugaru because we were more interested in the
game and the code seemed easier to understand. The build system also worked
better cross-platform, which was nice when we wanted to test things on our own
machines instead of the Jetsons.

Thankfully, the way that the Lugaru code was structured made it relatively easy
to extract useful quantities from the game. Most of the relevant variables are
defined in [Globals.cpp], and by redeclaring the variables elsewhere as
`extern`, we could use them in other files.

Then, once the game was completed (by having the player win or die), the
statistics are logged to a file `test.csv`. Some of these statistics were
provided directly in the game's code, while for others we had to devise our own
methods of obtaining them.

|Statistic     | Description |
---------------|--------------
user           | The profile name of the current user.
level          | The challenge level played during the test.
testFinish     | The time that the test ended (i.e., the data was logged.)
outcome        | Whether the player won or lost the challenge.
timeAlive      | The number of seconds that the challenge lasted.
numKicks       | The number of successful kicks landed by the player.
numRabbitKicks | The number of successful rabbit kicks (leaping kicks) landed by the player.
numAttacks     | The number of successful attacks by the player.
numReversals   | The number of successful reversals performed by the player.
numReversaled  | The number of successful reversals performed against the player.
difficulty     | The difficulty setting of the current player's profile.
score          | The player's final score after the challenge.

Every time a challenge completes, the game appends an additional line containing
each of these statistics to `tests.csv`. Our fork of Lugaru that performs the
logging may be found [here][lugaru-fork].

## Testing Methodology

For running the tests we had the following experiemental set up. We used three
players: myself (euclio), Bryan (Bmats), and a friend (Jono) to perform an ABBA
test of the controllers. In order to keep things as consistent as possible, we
played only challenge 1 on the easiest difficulty.

Each player played four rounds with one controller, then switched to the other
controller for 8 rounds, then switched back to the first controller.

Bryan and I have had similar experience with Lugaru, though our friend had never
played. However, we think that he picked up the game rather quickly.

## Results and Discussion

The raw data for [my controller] and [Bryan's controller] can be found in the
repo containing our fork of Lugaru.

Here is a summary of the averages for each set of data:

![](https://raw.githubusercontent.com/euclio/lugaru/master/Stats.png)

Given the data that we obtained, I would be inclined to say that Bryan's
controller performs slightly better than mine. Of course, his controller was
associated with more wins, but players were also more inclined to use reversals,
which turns out to be a very effective strategy in Lugaru. Interestingly, my
controller performed better in landing kicks and score (which are likely
related). This leads me to believe that controller may be slightly more precise
in performing kick actions or receiving bonuses.

## Conclusion

In total, Bryan and I spent about 11 and a half hours on this assignment. Most
of the time was dedicated to finding the correct parameters to log in the
codebase. We spent about 30 minutes performing the actual testing.

Unfortunately there are few conclusions that we can draw from our data. Some of
the factors that contributed to this is the fact that we were biased (Bryan and
I spent a lot of time testing on our own controllers) and that we did not have a
tremendous amount of data. There was also a skill gap between Bryan and I and
our friend, as Jono had never played Lugaru before. If we were to perform this
experiment again, we would definitely attempt to find more participants and
perhaps abstain from using our own controllers.

[lugaru-homepage]: http://www.wolfire.com/lugaru
[bryans-writeup]: https://github.com/ajinomoto124/E190uLab3/blob/master/lE190uLab3.md
[Globals.cpp]: https://github.com/euclio/lugaru/blob/master/Source/Globals.cpp
[my controller]: https://github.com/euclio/lugaru/blob/master/andy_tests.csv
[Bryan's controller]: https://github.com/euclio/lugaru/blob/master/tests.csv
[lugaru-fork]: https://github.com/euclio/lugaru
