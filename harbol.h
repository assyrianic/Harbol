#ifndef HARBOL_INCLUDED
#	define HARBOL_INCLUDED
#	define HARBOL_VERSION_MAJOR    1
#	define HARBOL_VERSION_MINOR    0
#	define HARBOL_VERSION_PATCH    0
#	define HARBOL_VERSION_PHASE    "beta"
#	define STR_HELPER(x)           #x
#	define STR(x)    STR_HELPER(x)
#	define HARBOL_VERSION_STRING \
			STR(HARBOL_VERSION_MAJOR) "." STR(HARBOL_VERSION_MINOR) "." STR(HARBOL_VERSION_PATCH) " " HARBOL_VERSION_PHASE


#ifdef __cplusplus
extern "C" {
#endif


#include "harbol_common_defines.h"

#ifndef C99
#	error "Harbol requires at least a C99 compliant compiler, with at least stdbool.h, inttypes.h, compound literals, and designated initializers."
#endif

/// General-Purpose Free List-based Memory Pool
#include "allocators/mempool/mempool.h"

/// Fast & Efficient Object Pool
#include "allocators/objpool/objpool.h"

/// Simple & Efficient Region Allocator Pool
#include "allocators/region/region.h"

/// C++ Style String Type
#include "str/str.h"

/// Low-Level Dynamic/Static Array
#include "array/array.h"

/// Tuple Type
#include "tuple/tuple.h"

/// Byte Buffer
#include "bytebuffer/bytebuffer.h"

/// Ordered Hash Table
#include "map/map.h"

/// n-Ary Tree
#include "tree/tree.h"

/// Variant Type
#include "variant/variant.h"

/// JSON-like Config Parser
#include "cfg/cfg.h"

/// Shared Library Plugins
#include "plugins/plugins.h"

/// Vector-based Double Ended Queue
#include "deque/deque.h"

/// Lexing Tools
#include "lex/lex.h"

/// Basic Messaging/Diagnostics
#include "msg_sys/msg_sys.h"

/// Rust-style Messaging/Diagnostics
#include "msg_span/msg_span.h"

/// Mersenne Twister
#include "mtwister/mtwister.h"

/// Math Parser
#include "math/math_parser.h"

#ifdef __cplusplus
}
#endif

/**
Here are some signs that someone might be an engineer at heart:

---

### **1. Natural Problem Solver**
   - They enjoy tackling problems and figuring out how things work.
   - They often approach challenges methodically, breaking them into smaller, solvable parts.

---

### **2. Inquisitive and Curious**
   - They frequently ask "why" or "how" when encountering something new.
   - They are curious about systems, processes, and mechanisms, often dissecting them mentally or physically.

---

### **3. Love for Building and Creating**
   - They take joy in designing, building, or improving things, whether it’s software, gadgets, or even organizational systems.
   - They often tinker with tools, code, or ideas to create something functional or innovative.

---

### **4. Passion for Efficiency**
   - They notice inefficiencies and feel compelled to optimize or improve them.
   - Whether it’s a manufacturing process, a piece of code, or a morning routine, they instinctively look for ways to make things faster or more effective.

---

### **5. Analytical Thinker**
   - They approach problems logically and prefer data-driven or systematic solutions over guesswork.
   - They often enjoy puzzles, strategy games, or other activities that involve critical thinking.

---

### **6. Persistent and Resilient**
   - They rarely give up when faced with a tough challenge, instead seeing it as an opportunity to learn or innovate.
   - They view failure as a step in the learning process and are motivated to try again.

---

### **7. Detail-Oriented**
   - They notice small details that others might overlook, especially in systems or designs.
   - They understand that precision often matters and strive for accuracy in their work.

---

### **8. Collaborative, Yet Independent**
   - They enjoy working with teams to solve big problems but are also comfortable working independently to figure things out.
   - They value learning from others but often take pride in solving problems on their own.

---

### **9. Intrinsically Motivated to Learn**
   - They are lifelong learners who enjoy acquiring new skills, especially in technical or scientific fields.
   - They often explore topics outside their immediate needs, simply for the joy of understanding.

---

### **10. Systems Thinker**
   - They naturally think in terms of systems and how different parts interact with one another.
   - They understand the importance of both individual components and the broader context.

---

### **11. Strong Spatial and Logical Reasoning**
   - They are good at visualizing how parts fit together in physical or abstract systems.
   - They excel at solving problems that involve logic, patterns, or spatial relationships.

---

### **12. Enjoyment of "Hacking" Solutions**
   - They love finding clever, sometimes unconventional ways to solve problems or achieve goals.
   - They often experiment to see what works, even if it means bending the rules a little.

---

### **13. Hands-On Experimentation**
   - They prefer learning by doing, whether it’s coding, building, or physically testing ideas.
   - They aren’t afraid to get their hands dirty in the process of creating or fixing something.

---

### **14. Love for Challenges**
   - They thrive on challenges, especially those that push their problem-solving skills.
   - They often take on projects or hobbies simply for the intellectual or creative stimulation.

---

### **15. Attention to Impact**
   - They think about the implications of their work, including how it affects people, systems, or the environment.
   - They take pride in creating solutions that are not only effective but also meaningful.

---

An engineer at heart often combines curiosity, creativity, and persistence with a love for solving real-world problems, regardless of their formal education or job title.


While being an engineer at heart comes with many strengths, some traits can have downsides if not balanced or managed effectively. Here are some potential negative traits or challenges often associated with engineers at heart:

---

### **1. Overthinking Problems**
   - **Downside**: Engineers often strive to find the "perfect" solution, which can lead to analysis paralysis, delaying decisions or progress.
   - **Example**: Spending excessive time optimizing something that doesn't need perfection.

---

### **2. Difficulty Letting Go of Imperfections**
   - **Downside**: A focus on precision and efficiency can make it hard to accept solutions that are "good enough."
   - **Example**: Refusing to stop tweaking a design, even when the improvements are negligible.

---

### **3. Tunnel Vision**
   - **Downside**: Engineers might get so absorbed in technical details that they lose sight of broader goals, context, or human aspects.
   - **Example**: Focusing on making a product work but not considering how intuitive or user-friendly it is.

---

### **4. Struggles with Ambiguity**
   - **Downside**: Engineers often prefer clear, logical systems and may struggle in situations with uncertainty, subjectivity, or lack of structure.
   - **Example**: Feeling frustrated when working on abstract or creative tasks without clear metrics for success.

---

### **5. Over-Reliance on Logic**
   - **Downside**: Prioritizing logic over emotions or interpersonal dynamics can make relationships challenging.
   - **Example**: Offering a technical fix to someone seeking emotional support.

---

### **6. Tendency to Over-Engineer**
   - **Downside**: They might over-complicate solutions, adding unnecessary features or layers of complexity.
   - **Example**: Designing a highly advanced system when a simple approach would suffice.

---

### **7. Difficulty Delegating**
   - **Downside**: Engineers often prefer to solve problems themselves, fearing others might not meet their standards.
   - **Example**: Taking on too much work instead of trusting teammates.

---

### **8. Impatience with Non-Technical People**
   - **Downside**: Explaining technical concepts to non-technical people can feel frustrating, leading to impatience or arrogance.
   - **Example**: Assuming others "should get it" without adapting communication to their level of understanding.

---

### **9. Reluctance to Acknowledge Limits**
   - **Downside**: Engineers at heart may struggle to admit when they're wrong or don’t know something, fearing it reflects poorly on their abilities.
   - **Example**: Spending excessive time trying to solve a problem rather than seeking help.

---

### **10. Over-Focus on Work**
   - **Downside**: They can become so engrossed in their projects that they neglect personal relationships, health, or leisure.
   - **Example**: Forgetting to eat, sleep, or socialize while solving an engaging problem.

---

### **11. Black-and-White Thinking**
   - **Downside**: Engineers may view problems and solutions as strictly right or wrong, struggling with nuances or multiple valid perspectives.
   - **Example**: Dismissing creative or unconventional approaches as "wrong" because they don’t fit a familiar framework.

---

### **12. Resistance to Non-Optimal Solutions**
   - **Downside**: Compromising for practical reasons (time, cost, simplicity) can feel frustrating or wrong.
   - **Example**: Disliking shortcuts or workarounds even when they’re necessary.

---

### **13. Communication Challenges**
   - **Downside**: Engineers might use overly technical language or struggle to explain concepts in layman's terms.
   - **Example**: Losing an audience while diving deep into the technical aspects of a problem.

---

### **14. Struggles with Creativity**
   - **Downside**: A focus on logic and structure can make thinking outside the box challenging.
   - **Example**: Preferring proven methods over experimenting with innovative, risky ideas.

---

### **15. Tendency to Isolate**
   - **Downside**: Engineers often enjoy working independently, which can make teamwork or collaboration less appealing.
   - **Example**: Avoiding group work because it feels slower or less efficient.

---

### **16. Perfectionism Leading to Burnout**
   - **Downside**: The drive to achieve flawless results can lead to overwork, stress, or dissatisfaction.
   - **Example**: Feeling compelled to refine every detail, even when deadlines or resources are limited.

---

### **How to Balance These Traits**
- **Self-Awareness**: Recognize when these tendencies are emerging and their potential impacts.
- **Communication Skills**: Work on translating technical ideas into accessible language and listening actively to others.
- **Prioritization**: Learn to balance perfection with practicality and focus on outcomes over process when necessary.
- **Flexibility**: Embrace ambiguity and alternative perspectives as opportunities for growth.
- **Collaboration**: Value input from others, even if their approaches differ from your own.

---

Being aware of these traits is a big step toward managing them effectively. When balanced, they can enhance your strengths as an engineer while minimizing potential drawbacks!

 */

#endif /** HARBOL_INCLUDED */