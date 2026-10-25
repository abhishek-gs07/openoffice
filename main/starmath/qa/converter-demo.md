# StarMath LaTeX Converter Demo

In the StarMath command editor, right-click and select **Convert from LaTeX**.
The examples below show the source text and the converted StarMath command.
The converter covers command aliases, grouped arguments, scripts, and
fractions; it is not a complete LaTeX environment or matrix parser.

| Formula | Before | After |
| --- | --- | --- |
| Quadratic formula | `x = \\frac{-b \\pm \\sqrt{b^2 - 4ac}}{2a}` | `x = {-b plusminus sqrt {b^2 - 4ac}} over {2a}` |
| Euler identity | `e^{i\\pi} + 1 = 0` | `e^{i pi} + 1 = 0` |
| Definite integral | `\\int_0^1 x^2\\,dx` | `int_0^1 x^2 dx` |
| Summation | `\\sum_{k=1}^{n} k` | `sum_{k=1}^{n} k` |
| Nested fraction | `\\frac{1}{1 + \\frac{1}{x}}` | `{1} over {1 + {1} over {x}}` |
| Pythagorean theorem | `a^2 + b^2 = c^2` | `a^2 + b^2 = c^2` |
| Gaussian integral | `\\int_{-\\infty}^{\\infty} e^{-x^2} dx` | `int_{-infinity}^{infinity} e^{-x^2} dx` |
| Matrix environment | `\\begin{bmatrix}a & b\\\\c & d\\end{bmatrix}` | `\\begin{bmatrix}a & b\\\\c & d\\end{bmatrix}` |
| Quadratic formula | `x = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}` | `x = {-b plusminus sqrt {b^2 - 4ac}} over {2a}` |
| Euler identity | `e^{i\pi} + 1 = 0` | `e^{i pi} + 1 = 0` |
| Definite integral | `\int_0^1 x^2\,dx` | `int _0^1 x^2 dx` |
| Summation | `\sum_{k=1}^{n} k` | `sum_{k=1}^{n} k` |
| Nested fraction | `\frac{1}{1 + \frac{1}{x}}` | `{1} over {1 + {1} over {x}}` |
| Gaussian integral | `\int_{-\infty}^{\infty} e^{-x^2} dx` | `int _{-infinity}^{infinity} e^{-x^2} dx` |
| Matrix environment | `\begin{bmatrix}a & b\\c & d\end{bmatrix}` | `\begin{bmatrix}a & b\\c & d\end{bmatrix}` |

Unknown commands and unsupported environments are preserved unchanged. The
matrix row is included as a pass-through example, not as a claim of matrix
conversion support.