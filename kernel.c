// kernel.c

void print(const char* str);
char get_char();
void clear_screen();
int strcmp(const char* a, const char* b);
void update_cursor(int x, int y);
void scroll();

// El puntero a la memoria de video
char* video_memory = (char*)0xb8000;
// Posición del cursor
int cursor_x = 0;
int cursor_y = 0;

void main() {
    clear_screen();
    print("Bienvenido a MiniShell OS!\nEscribe 'help' para ver los comandos.\n\n");

    char buffer[100];
    int index = 0;

    while (1) {
        print(">");
        index = 0;

        while (1) {
            char c = get_char();
            if (c == '\r') { // Tecla Enter
                buffer[index] = '\0';
                print("\n");
                break;
            } else if (c == '\b') { // Tecla Backspace
                if (index > 0) {
                    index--;
                    // Mueve el cursor hacia atrás y borra el carácter en la pantalla
                    if (cursor_x > 0) {
                        cursor_x--;
                        int offset = (cursor_y * 80 + cursor_x) * 2;
                        video_memory[offset] = ' ';
                        video_memory[offset + 1] = 0x07;
                        update_cursor(cursor_x, cursor_y);
                    }
                }
            } else {
                if (index < sizeof(buffer) - 1) {
                    buffer[index++] = c;
                    char str[2] = {c, '\0'};
                    print(str);
                }
            }
        }

        // Comandos
        if (strcmp(buffer, "help") == 0) {
            print("Comandos disponibles:\n");
            print("  help - Muestra esta ayuda\n");
            print("  clear - Limpia la pantalla\n");
        } else if (strcmp(buffer, "clear") == 0) {
            clear_screen();
        } else if (index > 0) {
            print("Comando no reconocido. Usa 'help'\n");
        }
    }
}

void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else {
            int offset = (cursor_y * 80 + cursor_x) * 2;
            video_memory[offset] = str[i];
            video_memory[offset + 1] = 0x07; // Atributo: texto blanco sobre fondo negro
            cursor_x++;
            if (cursor_x >= 80) {
                cursor_x = 0;
                cursor_y++;
            }
        }

        // Si el cursor está fuera de la pantalla, desplázala
        if (cursor_y >= 25) {
            scroll();
        }
    }
    update_cursor(cursor_x, cursor_y);
}

void clear_screen() {
    for (int i = 0; i < 80 * 25; ++i) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor(cursor_x, cursor_y);
}

void scroll() {
    // Mueve cada línea una posición hacia arriba
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 80; x++) {
            int to_offset = (y * 80 + x) * 2;
            int from_offset = ((y + 1) * 80 + x) * 2;
            video_memory[to_offset] = video_memory[from_offset];
            video_memory[to_offset + 1] = video_memory[from_offset + 1];
        }
    }

    // Limpia la última línea
    for (int x = 0; x < 80; x++) {
        int offset = (24 * 80 + x) * 2;
        video_memory[offset] = ' ';
        video_memory[offset + 1] = 0x07;
    }

    // Mueve el cursor al inicio de la última línea
    cursor_y = 24;
}

// Función para obtener un carácter del teclado usando interrupciones del BIOS
char get_char() {
    char c;
    __asm__ __volatile__ (
        "mov $0x00, %%ah\n\t"
        "int $0x16\n\t"
        "mov %%al, %0\n\t"
        : "=r"(c)
        :
        : "ax"
    );
    return c;
}

// Implementación estándar de strcmp
int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

// Función para actualizar el cursor de hardware (requiere E/S de puertos)
void update_cursor(int x, int y) {
    // La E/S de puertos está fuera del alcance de este ejemplo simple,
    // pero aquí es donde se implementaría la actualización del cursor parpadeante.
    // Se requeriría código assembly para escribir en los puertos 0x3D4 y 0x3D5.
}
