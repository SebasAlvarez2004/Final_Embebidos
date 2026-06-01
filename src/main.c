void spi_bus_init(void){
    spi_bus_config_t busconfig={
        .mosi_io_num=PIN_NUM_MOSI,
        .miso_io_num=PIN_NUM_MISO,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    return spi_bus_in   itialize(SPI_HOST, &busconfig, SPI_DMA_CH_AUTO);
};

spi_device_interface_config_t MCP4132 = {
    .clock_speed_hz = 1000
    .mode = 3,
    .spics_io_num = CS_SENSOR,
    .queue_size = 1,

};

//b : Escribir registro

void mcp4132_write_register(uint8_t reg, uint8_t addr){
    uint8_t tx_data[2] = {0};
    tx_data[0]=reg;
    tx_data[1] = addr;
    spi_transaction_t trans ={
        .length = 8*2
        .tx_buffer = tx_data,
    }
    spi_device_transmit(sensor_handle, &trans); //Sensor handle se entiende como el mcp4132
}
spi_sensor_write_register(SENSOR_CONFIG, 0x00);

//c : leer registro

uint8_t mcp4132_read_register(uint8_t reg){

    uint8_t tx_data[2] = {0};
    uint8_t rx_data[2] = {0};
    tx_data[0] = reg;
    tx_data[1] = 0x00;

    spi_transaction_t trans ={
        .length = 8*2,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    spi_device_transmit(sensor_handle, &trans);
    return rx_data[1];
}

//4. a
//Escribir directamente en el registro Wiper 0

void mcp4132_set_wiper(uint16_t addr, uint8_t* data){
    spi_memory_write_enable();
    uint8_t tx_data[4 + len]={0};
    tx_data[0] = addr; // Wiper0
    tx_data[1] = (addr >> 8) & 0xFF;
    tx_data[2] = addr & 0xFF;
    tx_data[3] = data;
    spi_transaction_t trans = {
        .length = 8 * 2,
        .tx_buffer = tx_data
    };
    spi_device_transmit(memory_handle, &trans);
}

//b 

void mcp4132_set_cutoff_frequency(uint16_t cutoff_freq){
    uint8_t data = (cutoff_freq >> 8) & 0xFF;

    const float c = 10e-9; //10nf
    const float pi = 3.1415;
    float Rwb = 1/(2*pi*cutoff_freq*c);
    //Aqui es poner las ecuaciones de frecuencia de corte para obtener el valor de la resistencia en digital (9-128) y enviarlo al potenciométro.
    int N = (Rwb-75)*128/(10e3);
    mcp4132_set_wiper(0x00, &data);

}

void app_main(void){
    spi_bus_init();
    spi_device_handle_t sensor_handle;
    spi_device_interface_config_t sensor_config = MCP4132;
    spi_bus_add_device(SPI_HOST, &sensor_config, &sensor_handle);

 
    mcp4132_write_register(SENSOR_CONFIG, 0x00); //Escribir

    uint8_t config_value = mcp4132_read_register(SENSOR_CONFIG); //Leer

    mcp4132_set_cutoff_frequency(1000);
}