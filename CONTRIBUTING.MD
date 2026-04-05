# Contribuir a EMZ-OS 🚀

¡Gracias por el interés en colaborar en **EMZ-OS**! Este es un proyecto de aprendizaje sobre arquitectura de sistemas x86 y desarrollo de kernels desde cero.

### 🛠️ Requisitos previos
Para compilar y probar tus cambios, asegúrate de tener:
* **GCC** (i686-elf preferiblemente)
* **NASM**
* **CMake** (mínimo 3.10)
* **QEMU** (qemu-system-i386)

---

### 📝 ¿Cómo empezar?
1. **Haz un Fork** del repositorio.
2. **Crea una rama** para tu mejora: `git checkout -b mejora-increible`.
3. **Realiza tus cambios** siguiendo las reglas de estilo (ver abajo).
4. **Prueba tu código** en QEMU antes de subirlo.
5. **Envía un Pull Request** explicando qué hace tu cambio.

---

### 📜 Reglas de Oro del Kernel
Como estamos en un entorno **Freestanding** (sin librerías estándar), ten en cuenta:
1. **Nada de `<stdio.h>` o `<stdlib.h>`**: Usa las librerías internas en `libs/`. Si falta algo, ¡prográmalo!
2. **Memoria**: Si usas `kmalloc()`, asegúrate de liberar la memoria con `kfree()` si es necesario para evitar leaks en el kernel.
3. **Arquitectura**: EMZ-OS es de **32 bits**. No uses tipos de datos de 64 bits a menos que sea estrictamente necesario para el hardware.
4. **Variables Globales**: Si usas una variable compartida entre archivos, márcala como `extern` en el header correspondiente.

---

### 📂 Estructura del Proyecto
* `/kernel`: Lógica principal del sistema y ciclo de vida.
* `/drivers`: Controladores de hardware (teclado, pantalla VGA, disco ATA).
* `/libs`: Funciones de utilidad (strings, memoria básica).
* `/cpu`: GDT, IDT y código en Assembly.
* `/boot`: Cargador de arranque.

---

### 💬 Estilo de Código
* Usa **4 espacios** para la sangría (indentación).
* Comenta las funciones críticas, especialmente si manipulan registros de la CPU o puertos E/S (`outb`, `inb`).
* Intenta mantener los nombres de variables en **inglés** o **español** de forma consistente (actualmente usamos una mezcla, ¡ayúdanos a unificar!).

---

### ⚠️ Reporte de Errores
Si encuentras un bug (como un Triple Fault o un Kernel Panic), abre un **Issue** describiendo:
1. Qué estabas haciendo cuando falló.
2. La captura de pantalla de QEMU (si es posible).
3. El commit exacto que estabas usando.