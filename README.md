# Custom-Built RTOS for ARM Cortex-M Microcontrollers

> A minimal real-time operating system (RTOS) developed from scratch in C and Assembly. This project serves educational purposes and aims to explore how an RTOS works at a low level.

---

## 🛠 Key Features

- ✅ Preemptive multitasking with priority-based scheduling  
- ✅ Manual context switching  
- ✅ Semaphores and Mutexes (with priority inheritance)  
- ✅ Message queues  
- ✅ Delay and sleep using SysTick  
- ✅ Written entirely in C and ARM Assembly, no OS dependencies

---

## ⚙️ System Requirements

- **Microcontroller**: STM32F103xx (ARM Cortex-M3)
- **Compiler & IDE**:  
  ❗ Only **directly compatible with IAR Embedded Workbench for ARM**

- **Required library**:  
  This project **must be used together with the `LHPstm32f103` library**, available on GitHub:
  
  👉 [https://github.com/hongphuclee/stm32f103c8t6_by_hpl](https://github.com/hongphuclee/stm32f103c8t6_by_hpl)

---

## ❌ Not compatible with STM32CubeIDE / Keil (out of the box)

This project is **not directly compatible with STM32CubeMX or Keil uVision**, due to assembly syntax tailored for IAR.  
To port the code, some changes in assembly declarations are required:

| IAR Syntax        | Keil / GCC (ARM) Equivalent |
|------------------|------------------------------|
| `RSEG`           | `AREA`                       |
| `EXTERN symbol`  | `.extern symbol`             |
| `PUBWEAK symbol` | `.weak symbol`               |
| `ENTRY`          | `_start:`                    |
| `END`            | Not needed or `.end`         |
| `CSEG`           | `AREA |.text|, CODE`         |

🔧 Other elements such as `LR`, `SP`, `R14`, etc., remain the same but may require syntax adjustment.

---

## 📚 References

- [ARM Cortex-M3 Technical Reference Manual](https://developer.arm.com/documentation/ddi0337/)
- [STM32 Cortex-M Programming Manual](https://www.st.com/resource/en/programming_manual/dm00046982.pdf)
- [LHPstm32f103 GitHub Repository](https://github.com/hongphuclee/stm32f103c8t6_by_hpl)

---

## 📌 Notes

- This project is **not intended for commercial use**.  
  It is designed for learning and research purposes only.
- The code prioritizes clarity and extensibility over performance optimization.

---

## 📧 Contact

If you have questions or want to contribute, feel free to open an [issue](https://github.com/hongphuclee/stm32f103c8t6_by_hpl/issues) or send a pull request.  
Contributions and feedback are welcome!
