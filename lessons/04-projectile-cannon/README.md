# Lesson 4: Projectiles and Angles (Cannon)

> Lesson 2 had gravity in one direction. Now the ball flies in **two** dimensions, launched at any angle, and you can *see* the physics as a trail.

<p align="center"><img src="../../docs/screenshots/cannon.png" width="600" alt="cannon_projectile.cpp"><br><sub>cannon_projectile.cpp, drawn by the real code</sub></p>

## 🎯 Goal

Turn an angle and a power into a velocity with `sin` and `cos`, simulate projectile motion, draw a trail with an array, and build a simple **HUD** (heads-up display).

## ▶️ Run it

```bat
build.bat lessons\04-projectile-cannon\cannon_projectile.cpp
```

| Key | Action |
|---|---|
| ← / → | Aim (5° to 85°) |
| ↑ / ↓ | Power |
| Space | Fire |
| Esc | Quit |

Hit the moving red target to score. The chain of circles shows the path of your last shot.

## 🧠 Concepts

### From angle + power to velocity

```
            /|
   power   / |  vy = power · sin(angle)   (up, so negative on screen!)
          /  |
         /___|
   vx = power · cos(angle)
```

```cpp
bvx =  power * cos(angle * PI / 180.0);   // C++ trig uses radians
bvy = -power * sin(angle * PI / 180.0);   // minus: screen y points down
```

Two classic bugs live in these two lines: forgetting to convert **degrees → radians**, and forgetting the **minus sign**.

### Projectile motion

Exactly the same update as the runner's jump, just with a horizontal speed too:

```cpp
bx += bvx;  by += bvy;  bvy += gravity;
```

The horizontal speed never changes and the vertical speed changes steadily, which is why the path is a **parabola**. Try 45°: with no air resistance it gives the longest range.

### Trails: drawing the past

Each frame the ball's position is appended to `trailX[]`/`trailY[]`, and all saved positions are drawn as circles. Because the ball moves equal *time* steps, the circles spread out where the ball is fast and bunch up near the top of the arc where it is slow. You are looking at a physics graph!

### HUD

Score, angle and power are drawn last, on a white panel, so they always sit on top. `sprintf` formats numbers into text for `outtextxy`.

## 🔍 Code tour

| Look for | Concept |
|---|---|
| `// ---- Aim controls ...` | Clamping values to a range |
| `// ---- Fire ----` | Angle → velocity |
| `trailCount < MAX_TRAIL` | Fixed-size array as a history buffer |
| `hitFlash` | A countdown timer for a visual effect |
| `Beep(1000, 100)` | Simple sound effect |

## 🛠️ Exercises

- 🟢 **Warm-up:** Draw a dotted "aim preview" line showing the first 10 positions the ball *would* have if fired now.
- 🟡 **Challenge:** Add wind: a random horizontal acceleration each shot, shown as an arrow on the HUD.
- 🔴 **Boss level:** Make the ball **bounce** on the ground (flip `bvy`, multiply by 0.7) up to three times before it stops. You will meet the general version of this in [Lesson 6](../06-collision-lab/).

**Next →** [Lesson 5: building Bomberman step by step](../05-bomberman/)
