//===================================================================================================
// Class NetAnswerSetCoolingValue
//===================================================================================================
/****************************************************************************************************
 * \fn NetAnswerSetCoolingValue()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerSetCoolingValue::NetAnswerSetCoolingValue()
{
    m_packet_name = "Answer SetCoolingValue";
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerSetCoolingValue::log() const
{
    std::cout << "-- NetAnswerSetCoolingValue content --" << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerSetCoolingValue::totalLog() const
{
    NetAnswerCommandDone::totalLog();
    NetAnswerSetCoolingValue::log();
}