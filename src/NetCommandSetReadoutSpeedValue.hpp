//===================================================================================================
// Class NetCommandSetReadoutSpeedValue
//===================================================================================================
/****************************************************************************************************
 * \fn NetCommandSetReadoutSpeedValue()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetCommandSetReadoutSpeedValue::NetCommandSetReadoutSpeedValue()
{
    m_function_number   = NetCommandHeader::g_function_number_set_single_parameter; // function to be executed (1000 .. 1999)
    m_packet_name       = "Command SetReadoutSpeedValue";
    m_is_server_command = false; // some commands are server related, others to a camera
    m_readout_speed_value = 0  ;
    m_readout_speed_name = "DSI Sample Time\0";
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetCommandSetReadoutSpeedValue::size() const
{
    return  sizeof(m_readout_speed_value) + m_readout_speed_name.size();
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetCommandSetReadoutSpeedValue::totalSize() const
{   
    return NetCommandHeader::totalSize() + NetCommandSetReadoutSpeedValue::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetReadoutSpeedValue::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size != NetCommandSetReadoutSpeedValue::size())
        return false;

    readData(in_out_memory_data, m_readout_speed_value);

    in_out_memory_size -= NetCommandSetReadoutSpeedValue::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetReadoutSpeedValue::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetCommandSetReadoutSpeedValue::size())
        return false;

    writeData(in_out_memory_data, m_readout_speed_value);
    memcpy(reinterpret_cast<char *>(in_out_memory_data), m_readout_speed_name.c_str(), m_readout_speed_name.size());  

    in_out_memory_size -= NetCommandSetReadoutSpeedValue::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetReadoutSpeedValue::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetCommandHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandSetReadoutSpeedValue::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_out_memory_size) const
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetReadoutSpeedValue::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetCommandHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandSetReadoutSpeedValue::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandSetReadoutSpeedValue::log() const
{
    std::cout << "-- NetCommandSetReadoutSpeedValue content --" << std::endl;
    std::cout << "m_readout_speed_value : " << m_readout_speed_value << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandSetReadoutSpeedValue::totalLog() const
{
    NetCommandHeader::totalLog();
    NetCommandSetReadoutSpeedValue::log();
}

//###########################################################################