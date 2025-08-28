#include <Arduino.h>

// Cấu hình chân điều khiển
const int PUL = 2; // Chân xung (Pulse)
const int DIR = 3; // Chân hướng (Direction)
const int ENA = 4; // Chân kích hoạt (Enable)

// Thông số động cơ Nema 17
const int STEP_PER_REV = 200; // Số bước trên một vòng
const int MICROSTEPS = 16; // Số microsteps trên một bước
const int TOTAL_STEPS = STEP_PER_REV * MICROSTEPS; // Tổng số bước trên 1 vòng
const float DEGREES_PER_STEP = 360.0 / TOTAL_STEPS; // Số độ trên một bước

// Biến điều khiển
float currentPos = 0.0; // Vị trí hiện tại (độ)
int stepDelay = 50; // Delay giữa các bước (microseconds)
bool motorOn = true; // Trạng thái động cơ

// Test động cơ
void testMotor() {
    if (!motorOn) {
        Serial.println("❌ Động cơ chưa được kích hoạt");
        return;
    }

    Serial.println("✅ Bắt đầu test động cơ");

    // Test chiều thuận
    Serial.println("Test thuận chiều 32000 bước");
    digitalWrite(DIR, HIGH);
    delayMicroseconds(10);

    for (int i = 0; i < 32000; i++) {
        digitalWrite(PUL, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(PUL, LOW);
        delayMicroseconds(stepDelay);

        if ((i + 1) % 3200 == 0) {
            Serial.print("Bước : ");
            Serial.println(i + 1);
        }
    }

    Serial.println("✅ Hoàn thành chiều thuận");
    delay(1000);

    // Test chiều ngược
    Serial.println("Test ngược chiều 32000 bước");
    digitalWrite(DIR, LOW);
    delayMicroseconds(10);

    for (int i = 0; i < 32000; i++) {
        digitalWrite(PUL, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(PUL, LOW);
        delayMicroseconds(stepDelay);

        if ((i + 1) % 3200 == 0) {
            Serial.print("Bước : ");
            Serial.println(i + 1);
        }
    }

    Serial.println("✅ Hoàn thành chiều nghịch");
    delay(1000);
}

// Move theo số bước đầu vào
void moveSteps(int steps, bool clockwise) {
    if (!motorOn || steps <= 0) return;

    // Đặt hướng
    digitalWrite(DIR, clockwise ? HIGH : LOW); // HIGH = thuận, LOW = nghịch
    delayMicroseconds(10); // Đợi tín hiệu ổn định

    // Thực hiện các bước
    for (int i = 0; i < steps; i++) {
        digitalWrite(PUL, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(PUL, LOW);
        delayMicroseconds(stepDelay);
        
        // Hiển thị tiến trình cho số bước lớn
        if (steps > 50 && (i + 1) % (steps / 10) == 0) {
            Serial.print(".");
        }
    }
    if (steps > 50) {
        Serial.println(); // Xuống dòng sau dấu chấm khi thực hiện xong tổng số bước
    }
}

// Quay đến góc tuyệt đối
void rotateToAngle(float targetAngle) {
    if (!motorOn) {
        Serial.println("❌ Động cơ chưa được kích hoạt");
        return;
    }

    // Chuẩn hóa góc đích
    while (targetAngle >= 360) targetAngle -= 360;
    while (targetAngle < 0) targetAngle += 360;

    // Tính góc cần quay
    float angleDiff = targetAngle - currentPos;

    // Chọn đường đi ngắn nhất
    if (angleDiff > 180) angleDiff -= 360; // Khi quay thuận chiều quá lớn, chuyển sang ngược chiều
    if (angleDiff < -180) angleDiff += 360; // Khi quay ngược chiều quá lớn, chuyển sang thuận chiều

    if (abs(angleDiff) < DEGREES_PER_STEP) {
        Serial.println("✅ Đã ở vị trí mục tiêu");
        return;
    }

    bool clockwise = angleDiff > 0;
    int steps = round(abs(angleDiff) / DEGREES_PER_STEP);

    Serial.print("Từ ");
    Serial.print(currentPos, 2);
    Serial.print("° đến ");
    Serial.print(targetAngle, 2);
    Serial.print("° (");
    Serial.print(steps);
    Serial.print(" bước ");
    Serial.print(clockwise ? "thuận" : "ngược");
    Serial.println(" chiều)");

    moveSteps(steps, clockwise);
    currentPos = targetAngle;

    Serial.print("✅ Đã move đến vị trí : ");
    Serial.print(currentPos, 2);
    Serial.println("°");
}

// Quay đến góc tương đối
void rotateRelative(float angle, bool clockwise) {
    if (!motorOn) {
        Serial.println("❌ Động cơ chưa được kích hoạt");
        return;
    }

    int steps = round(angle / DEGREES_PER_STEP);
    moveSteps(steps, clockwise);

    // Cập nhật vị trí
    if (clockwise) {
        currentPos += angle;
    }
    else {
        currentPos -= angle;
    }

    // Chuẩn hóa vị trí (0 - 360)
    while (currentPos >= 360) currentPos -= 360;
    while (currentPos < 0) currentPos += 360;

    Serial.print("✅ Vị trí mới : ");
    Serial.print(currentPos, 2);
    Serial.println("°");
}

// Kiểm tra góc hợp lệ
bool isValidAngle(float angle) {
    if (angle <0 || angle > 360) {
        Serial.println("❌ Góc phải từ 0-360°");
        return false;
    }
    return true;
}

// Đặt tốc độ
void setSpeed(int speed) {
    if(speed >= 200 && speed <= 2000) {
        stepDelay = speed;
        Serial.print("✅ Tốc độ đã đặt : ");
        Serial.print(speed);
        Serial.println(" μs");
    }
    else {
        Serial.println("❌ Tốc độ phải từ 200-2000 μs");
    }
}

// Kích hoạt động cơ
void enableMotor() {
    motorOn = true;
    digitalWrite(ENA, HIGH);
    Serial.println("✅ Động cơ đã được kích hoạt");
}

// Tắt động cơ
void disableMotor() {
    motorOn = false;
    digitalWrite(ENA, LOW);
    Serial.println("⚠️ Động cơ đã được tắt");
}

// Về vị trí gốc
void goHome() {
    Serial.println("🏠 Đang về vị trí góc");
    rotateToAngle(0);
}

// Reset vị trí
void resetPosition() {
    currentPos = 0.0;
    Serial.println("🔄 Vị trí đã được reset về 0°");
}

// Hiển thị trạng thái
void printStatus() {
    Serial.println("\n========== Trạng Thái ==========");
    Serial.print("Vị trí hiện tại : ");
    Serial.print(currentPos, 2);
    Serial.println("°");
    Serial.print("Tốc độ : ");
    Serial.print(stepDelay);
    Serial.println(" μs");
    Serial.print("Độ chính xác : ");
    Serial.print(DEGREES_PER_STEP, 4);
    Serial.println("°/bước");
    Serial.print("Trạng thái : ");
    Serial.println(motorOn ? "Kích hoạt" : "Tắt");
    Serial.println("==============================");
}

// Hiển thị Menu
void printMenu() {
    Serial.println("\n==========Menu Điều Khiển==========");
    Serial.println("Các lệnh có sẵn : ");
    Serial.println("<góc>               - Quay đến góc (0-360°)");
    Serial.println("+<góc>              - Quay thuận chiều");
    Serial.println("-<góc>              - Quay ngược chiều");
    Serial.println("speed <giá_trị>     - Đặt tốc độ (200-2000μs)");
    Serial.println("home                - Về vị trí home");
    Serial.println("pos                 - Hiển thị vị trí");
    Serial.println("reset               - Reset vị trí về 0");
    Serial.println("enable              - Kích hoạt động cơ");
    Serial.println("disable             - Tắt động cơ");
    Serial.println("status              - Hiển thị trạng thái");
    Serial.println("test                - Test động cơ");
    Serial.println("menu                - Hiển thị menu");
    Serial.println("====================================");
    Serial.println("Nhập lệnh : ");
}

// Xử lý lệnh
void processCommand(String command) {
    command.toLowerCase();

    // Lệnh với ký tự đặc biệt
    if(command.startsWith("+")) {
        float angle = command.substring(1).toFloat();
        if (isValidAngle(angle)) {
            Serial.print("Quay thuận chiều ");
            Serial.print(angle);
            Serial.println("°");
            rotateRelative(angle, true);
        }
    }
    else if (command.startsWith("-")) {
        float angle = command.substring(1).toFloat();
        if (isValidAngle(angle)) {
            Serial.print("Quay ngược chiều ");
            Serial.print(angle);
            Serial.println("°");
            rotateRelative(angle, false);
        }
    }
    else if (command.startsWith("speed")) {
        int speed = command.substring(6).toInt();
        setSpeed(speed);
    }

    // Lệnh từ khóa
    else if (command == "home") {
        goHome();
    }
    else if (command == "pos") {
        Serial.print("Vị trí hiện tại : ");
        Serial.print(currentPos, 2);
        Serial.println("°");
    }
    else if (command == "reset") {
        resetPosition();
    }
    else if (command == "enable") {
        enableMotor();
    }
    else if (command == "disable") {
        disableMotor();
    }
    else if (command == "status") {
        printStatus();
    }
    else if (command == "test") {
        testMotor();
    }
    else if (command == "menu") {
        printMenu();
    }
    else {
        // Góc tuyệt đối
        float angle = command.toFloat();
        if (isValidAngle(angle)) {
            Serial.print("Quay đến ");
            Serial.print(angle);
            Serial.println("°");
            rotateToAngle(angle);
        }
        else {
            Serial.println("❌ Lệnh không hợp lệ. Gõ menu để xem hướng dẫn");
        }
    }
}

void setup() {
    // Khởi tạo serial
    Serial.begin(9600);

    // Cấu hình chân
    pinMode(PUL, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(ENA, OUTPUT);

    // Khởi tạo trạng thái
    digitalWrite(PUL, LOW);
    digitalWrite(DIR, LOW);
    digitalWrite(ENA, HIGH); // Kích hoạt động cơ

    // Hiển thị thông tin khởi động
    Serial.println("========================================");
    Serial.println("Điều Khiển Động Cơ Bước Nema 17");
    Serial.println("========================================");
    Serial.print("Tổng số bước/vòng : ");
    Serial.println(TOTAL_STEPS);
    Serial.print("Độ chính xác : ");
    Serial.print(DEGREES_PER_STEP, 4);
    Serial.print("°/bước");
    Serial.print("Tốc độ hiện tại : ");
    Serial.print(stepDelay);
    Serial.println(" μs");

    // Hiển thị menu
    printMenu();
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if(command.length() > 0) {
            processCommand(command);
        }
    }
}