#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialPort(new QSerialPort(this))
{
    ui->setupUi(this);

    // 初始化波特率
    ui->baudRateComboBox->addItems({"4800", "9600", "19200", "38400", "57600", "115200"});
    ui->baudRateComboBox->setCurrentText("9600");

    // 算法类型
    ui->algorithmComboBox->addItem("逐点比较法", 0x01);
    ui->algorithmComboBox->addItem("数字积分法（DDA）", 0x02);
    ui->algorithmComboBox->addItem("Bresenham算法", 0x03);

    // 细分值（常见驱动细分数）
    ui->subdivComboBox->addItems({"1", "2", "4", "8", "16", "32", "64", "128", "256"});

    // 插补类型
    ui->interpTypeComboBox->addItem("直线插补", 0x00);  // 0x00：直线
    ui->interpTypeComboBox->addItem("圆弧插补（顺）", 0x10); // 高 4 位 1 表示圆弧，低 4 位 0 表示顺
    ui->interpTypeComboBox->addItem("圆弧插补（逆）", 0x11); // 高 4 位 1 表示圆弧，低 4 位 1 表示逆

    // 象限选择
    ui->quadrantComboBox->addItem("第一象限", 1);
    ui->quadrantComboBox->addItem("第二象限", 2);
    ui->quadrantComboBox->addItem("第三象限", 3);
    ui->quadrantComboBox->addItem("第四象限", 4);

    // 刷新串口列表
    refreshSerialPorts();
    ui->commandoutputBox->setReadOnly(true);

    // 串口接收信号连接
    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialReceived);


    // 设置窗口标题
    setWindowTitle("串口助手");
}

MainWindow::~MainWindow()
{
    if (serialPort->isOpen())
        serialPort->close();
    delete ui;
}

void MainWindow::refreshSerialPorts()
{
    ui->serialbox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        ui->serialbox->addItem(info.portName());
    }
    if (!ports.isEmpty()) {
        ui->serialbox->setCurrentIndex(0);
    }
}

void MainWindow::on_refreshButton_clicked()
{
    refreshSerialPorts();
}

void MainWindow::on_serialbox_currentTextChanged(const QString &arg1)
{
    qDebug() << "用户选择串口：" << arg1;
}

void MainWindow::on_baudRateComboBox_currentTextChanged(const QString &arg1)
{
    qDebug() << "选择波特率：" << arg1;
    // 实际设置串口波特率在连接时处理
}

void MainWindow::on_connectButton_clicked()
{
    if (!isConnected) {
        if (serialPort->isOpen())
            serialPort->close();

        serialPort->setPortName(ui->serialbox->currentText());

        // 设置波特率
        QSerialPort::BaudRate baud;
        QString baudStr = ui->baudRateComboBox->currentText();
        if (baudStr == "4800") baud = QSerialPort::Baud4800;
        else if (baudStr == "9600") baud = QSerialPort::Baud9600;
        else if (baudStr == "19200") baud = QSerialPort::Baud19200;
        else if (baudStr == "38400") baud = QSerialPort::Baud38400;
        else if (baudStr == "57600") baud = QSerialPort::Baud57600;
        else if (baudStr == "115200") baud = QSerialPort::Baud115200;
        else baud = QSerialPort::Baud9600;

        serialPort->setBaudRate(baud);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setParity(QSerialPort::NoParity);

        if (serialPort->open(QIODevice::ReadWrite)) {
            isConnected = true;
            ui->connectButton->setText("断开连接");
            QMessageBox::information(this, "提示", "串口连接成功");
        } else {
            QMessageBox::critical(this, "错误", "串口连接失败: " + serialPort->errorString());
        }
    } else {
        serialPort->close();
        isConnected = false;
        ui->connectButton->setText("连接");
        QMessageBox::information(this, "提示", "串口已断开连接");
    }
}

void MainWindow::on_algorithmComboBox_currentTextChanged(const QString &arg1)
{
    uint8_t value = ui->algorithmComboBox->currentData().toUInt();
    qDebug() << "[算法类型] 选择：" << arg1 << "，编码值：" << value;
}

void MainWindow::on_subdivComboBox_currentTextChanged(const QString &arg1)
{
    uint16_t subdiv = arg1.toUShort();
    qDebug() << "[细分值] 选择：" << arg1 << "，数值：" << subdiv;
}

void MainWindow::on_interpTypeComboBox_currentTextChanged(const QString &arg1)
{
    uint8_t value = ui->interpTypeComboBox->currentData().toUInt();
    qDebug() << "[插补类型] 选择：" << arg1 << "，编码值：" << value;
}

void MainWindow::on_quadrantComboBox_currentTextChanged(const QString &arg1)
{
    uint8_t value = ui->quadrantComboBox->currentData().toUInt();
    qDebug() << "[象限] 选择：" << arg1 << "，编码值：" << value;
}



void MainWindow::on_x_s_textChanged(const QString &arg1)
{
    x_start = arg1.toInt();
}

void MainWindow::on_y_s_textChanged(const QString &arg1)
{
    y_start = arg1.toInt();
}

void MainWindow::on_x_f_textChanged(const QString &arg1)
{
    x_end = arg1.toInt();
}

void MainWindow::on_y_f_textChanged(const QString &arg1)
{
    y_end = arg1.toInt();
}


void MainWindow::on_run_clicked()
{
    if (!serialPort || !serialPort->isOpen()) {
        QMessageBox::warning(this, "提示", "请先连接串口！");
        return;
    }

    uint8_t cmd = 0x00;  // 初始化一个默认值              // 命令码：01=直线，02=圆弧
    QByteArray payload;

    uint8_t algorithm = ui->algorithmComboBox->currentData().toUInt();
    uint8_t quadrant  = ui->quadrantComboBox->currentData().toUInt();
    uint16_t subdiv   = ui->subdivComboBox->currentText().toUShort();
    uint16_t speed = ui->speedbox->text().toUShort();  // 替代原来的固定值

    int interpType = ui->interpTypeComboBox->currentData().toInt();

    if (interpType == 0x00) {
        // === 构建直线插补指令 ===
        cmd = 0x01;
        int32_t xDelta = ui->x_s->text().toInt();
        int32_t yDelta = ui->y_s->text().toInt();

        payload.append(algorithm);
        payload.append(quadrant);
        // 虚拟的“方向”，占位，设为0x00
        payload.append(static_cast<char>(0x00));

        // 起点X/Y 视为 xDelta/yDelta
        for (int i = 3; i >= 0; --i) payload.append((xDelta >> (8 * i)) & 0xFF);
        for (int i = 3; i >= 0; --i) payload.append((yDelta >> (8 * i)) & 0xFF);

        // 终点X/Y 同起点，或设为 0
        for (int i = 0; i < 8; ++i) payload.append('\0');

        // 圆心 X/Y 全部补 0
        for (int i = 0; i < 8; ++i) payload.append('\0');

        // 速度
        payload.append((speed >> 8) & 0xFF);
        payload.append(speed & 0xFF);

        // 细分值（8位无符号整数）
        payload.append(subdiv);
    }
        else if ((interpType & 0xF0) == 0x10) {
            // === 圆弧插补 ===
            cmd = 0x02;
            uint8_t direction = (interpType & 0x0F) == 0x01 ? 0x01 : 0x00;

            int32_t xStart = ui->x_s->text().toInt();
            int32_t yStart = ui->y_s->text().toInt();
            int32_t xEnd   = ui->x_f->text().toInt();
            int32_t yEnd   = ui->y_f->text().toInt();
            int32_t xC     = ui->x_c->text().toInt();  // 新增圆心
            int32_t yC     = ui->y_c->text().toInt();

            payload.append(algorithm);
            payload.append(quadrant);
            payload.append(direction);

            for (int i = 3; i >= 0; --i) payload.append((xStart >> (8 * i)) & 0xFF);
            for (int i = 3; i >= 0; --i) payload.append((yStart >> (8 * i)) & 0xFF);
            for (int i = 3; i >= 0; --i) payload.append((xEnd   >> (8 * i)) & 0xFF);
            for (int i = 3; i >= 0; --i) payload.append((yEnd   >> (8 * i)) & 0xFF);
            for (int i = 3; i >= 0; --i) payload.append((xC     >> (8 * i)) & 0xFF);
            for (int i = 3; i >= 0; --i) payload.append((yC     >> (8 * i)) & 0xFF);

            payload.append((speed >> 8) & 0xFF);
            payload.append(speed & 0xFF);
            // 细分值（8位无符号整数）
            payload.append(subdiv);
        }

    // === 构建最终帧 ===
    QByteArray packet;
    packet.append(0xAA);
    packet.append(0x55);
    packet.append(cmd);
    packet.append(0x1F); // 数据长度固定为31字节（0x1F）
    packet.append(payload);

    // 校验和（累加所有字节）
    uint8_t checksum = 0;
    for (const auto& b : packet)
        checksum += static_cast<uint8_t>(b);
    packet.append(checksum);

    // 发送串口
    serialPort->write(packet);

    // 打印到窗口
    QString hexStr = packet.toHex(' ').toUpper();
    if (ui->commandoutputBox)
        ui->commandoutputBox->appendPlainText("[发送] " + hexStr);

    qDebug() << "[上位机发送]：" << hexStr;
}



void MainWindow::on_draw_clicked()
{
    QLineSeries *series = new QLineSeries();

    int interpType = ui->interpTypeComboBox->currentData().toInt();

    if (interpType == 0x00) {
        // 直线插补
        int x0 = ui->x_s->text().toInt();
        int y0 = ui->y_s->text().toInt();
        int x1 = ui->x_f->text().toInt();
        int y1 = ui->y_f->text().toInt();

        *series << QPointF(x0, y0) << QPointF(x1, y1);

    } else if ((interpType & 0xF0) == 0x10) {
        // 圆弧插补
        int x0 = ui->x_s->text().toInt();
        int y0 = ui->y_s->text().toInt();
        int x1 = ui->x_f->text().toInt();
        int y1 = ui->y_f->text().toInt();
        int xc = ui->x_c->text().toInt();
        int yc = ui->y_c->text().toInt();

        int numSegments = 100;
        double r = std::hypot(x0 - xc, y0 - yc);
        double theta0 = atan2(y0 - yc, x0 - xc);
        double theta1 = atan2(y1 - yc, x1 - xc);

        bool clockwise = ((interpType & 0x0F) == 0x01);
        double deltaTheta = theta1 - theta0;
        if (clockwise && deltaTheta > 0) deltaTheta -= 2 * M_PI;
        if (!clockwise && deltaTheta < 0) deltaTheta += 2 * M_PI;

        for (int i = 0; i <= numSegments; ++i) {
            double t = theta0 + deltaTheta * i / numSegments;
            double px = xc + r * cos(t);
            double py = yc + r * sin(t);
            *series << QPointF(px, py);
        }
    }

    // 构建 chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->hide();
    chart->setTitle("轨迹插补图");

    // 设置 chart 到 QChartView（即提升后的 chartView）
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}


void MainWindow::on_clear_clicked()
{
    if (!ui->chartView) return;

    // 创建一个新的空图表
    QChart *emptyChart = new QChart();
    emptyChart->setTitle("已清除绘图");
    emptyChart->legend()->hide();

    // 设置到 chartView
    ui->chartView->setChart(emptyChart);
}

void MainWindow::onSerialReceived()
{
    QByteArray data = serialPort->readAll();

    // 可选：将 data 按 hex 显示
    QString hexStr = data.toHex(' ').toUpper();
    ui->commandoutput->appendPlainText("[接收] " + hexStr);

    // 如果需要按文本解析也可以这样做：
    // QString str = QString::fromUtf8(data);
    // ui->commandoutputBox->appendPlainText("[接收] " + str);

    qDebug() << "[接收到数据]：" << hexStr;
}
