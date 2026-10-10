# 🏛️ The Bug Museum

Every exhibit here is a real bug that appears (or is carefully avoided) in the practice code of this repository. Learn to recognize them and you will save hours.

Each exhibit has a **symptom** (what you see), a **cause** (what is really happening) and a **cure** (how the code in this repo deals with it).

---

### Exhibit 1: The Flickering Screen

- **Symptom:** Objects blink or shimmer while moving.
- **Cause:** Every `cleardevice()` and every shape is shown on the real screen immediately, so you sometimes see a half-drawn frame.
- **Cure:** Draw to a hidden buffer and show the finished frame at once:
  ```cpp
  BeginBatchDraw();
  while (running) { cleardevice(); /* draw */ FlushBatchDraw(); Sleep(16); }
  EndBatchDraw();
  ```
- **Seen in:** every lesson from 1 onward.

### Exhibit 2: The Machine-Gun Spacebar

- **Symptom:** Tapping Space fires 5 arrows.
- **Cause:** `GetAsyncKeyState` reports the key as down on *every frame* you hold it.
- **Cure:** React only when the key goes from up to down (`spaceNow && !prevSpace`).
- **Seen in:** [Lesson 3](../lessons/03-many-objects-archery/), [Lesson 4](../lessons/04-projectile-cannon/).

### Exhibit 3: The Runaway Score

- **Symptom:** Passing one pillar gives +40 points.
- **Cause:** The "pillar is behind the player" check is true for many frames in a row.
- **Cure:** A `counted` flag, set when scoring and reset when a new pillar appears.
- **Seen in:** [Lesson 2](../lessons/02-game-loop-runner/).

### Exhibit 4: The Upside-Down Jump

- **Symptom:** Pressing jump sends the player into the ground.
- **Cause:** Screen `y` grows **downward**, so "up" is negative.
- **Cure:** Jump with `vy = -14`, and launch with `bvy = -power * sin(angle)`.
- **Seen in:** [Lesson 2](../lessons/02-game-loop-runner/), [Lesson 4](../lessons/04-projectile-cannon/).

### Exhibit 5: The Degrees-vs-Radians Trap

- **Symptom:** The cannon fires in strange directions that do not match the angle on screen.
- **Cause:** `sin()` and `cos()` take **radians**, but humans think in degrees.
- **Cure:** Convert: `angle * PI / 180.0`.
- **Seen in:** [Lesson 4](../lessons/04-projectile-cannon/).

### Exhibit 6: The Ghost (Tunneling)

- **Symptom:** A fast ball passes straight through a wall.
- **Cause:** Movement happens in jumps. If one frame's jump is bigger than the wall is thick, the ball is never seen *touching* the wall.
- **Cure:** **Sub-stepping**: split each frame's movement into small steps and check collisions after each one.
- **Seen in:** `UpdateBall()` in [Lesson 6](../lessons/06-collision-lab/).

### Exhibit 7: The Sticky Wall

- **Symptom:** The ball hits a surface and jitters on it, or sinks into it.
- **Cause:** After a bounce the ball is still overlapping the surface, so next frame it bounces *again*, back into the wall.
- **Cure:** Two parts. Only reflect if the ball is moving *into* the surface (`vn < 0`), then push it out to exactly one radius away (`b.x = qx + nx * BALL_R`).
- **Seen in:** both programs in [Lesson 6](../lessons/06-collision-lab/).

### Exhibit 8: The Same Game Every Time

- **Symptom:** The "random" map is identical on every run.
- **Cause:** `rand()` always starts from the same seed.
- **Cure:** Call `srand((unsigned)time(NULL));` **once**, at the start of `main`. (Calling it every frame is a bug too: the numbers stop looking random.)
- **Seen in:** [Lesson 5](../lessons/05-bomberman/) and most other lessons.

### Exhibit 9: The Lane Lock (a puzzle for you)

- **Symptom:** In Bomberman step 3, walk to the very top row. Now try to move left or right.
- **Cause:** Lane centers are at `y = 25, 125, 225…`, but the edge clamp sets `y = r`, which is `20`. Now `(y - 25) % 100` is not `0`, so horizontal moves are refused until you step down again.
- **Cure:** Your turn! Hint: what value should the clamp use so the player stays on a lane center?
- **Seen in:** `KeyControl()` in [Lesson 5](../lessons/05-bomberman/).

### Exhibit 10: The Double Hit (another puzzle)

- **Symptom:** Rarely, one arrow pops two balloons at once.
- **Cause:** After an arrow hits, it is marked inactive, but the loop keeps testing it against the remaining balloons.
- **Cure:** Your turn! One `break;` in the right place.
- **Seen in:** the collision loop in [Lesson 3](../lessons/03-many-objects-archery/).

---

## 🔦 A debugging checklist

When something is wrong, go through this list before changing random code:

1. **Can I reproduce it?** Find the exact steps that make it happen.
2. **Which loop step is it in?** Input, update or draw? Comment out parts to narrow it down.
3. **Print the numbers.** Show the suspicious variable on screen with `sprintf` + `outtextxy`, or step through with the debugger (F5 in VS Code).
4. **Slow it down.** Increase `Sleep()` to 200 ms and watch frame by frame.
5. **Fix one thing, then run again.**
6. **Write it down** in your [weekly report](../templates/weekly-report.md). Future you will thank you.

Found a new bug? Add an exhibit!
