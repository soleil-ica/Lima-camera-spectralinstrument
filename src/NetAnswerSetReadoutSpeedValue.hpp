//===================================================================================================
// Class NetAnswerSetReadoutSpeedValue
//===================================================================================================
/****************************************************************************************************
 * \fn NetAnswerSetReadoutSpeedValue()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerSetReadoutSpeedValue::NetAnswerSetReadoutSpeedValue()
{
    m_packet_name = "Answer SetReadoutSpeedValue";
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerSetReadoutSpeedValue::log() const
{
    std::cout << "-- NetAnswerSetReadoutSpeedValue content --" << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerSetReadoutSpeedValue::totalLog() const
{
    NetAnswerCommandDone::totalLog();
    NetAnswerSetReadoutSpeedValue::log();
}