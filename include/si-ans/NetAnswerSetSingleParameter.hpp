//===================================================================================================
// Class NetAnswerSetSingleParameter
//===================================================================================================
/****************************************************************************************************
 * \fn NetAnswerSetSingleParameter()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerSetSingleParameter::NetAnswerSetSingleParameter()
{
    m_packet_name = "Answer SetSingleParameter";
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerSetSingleParameter::log() const
{
    std::cout << "-- NetAnswerSetSingleParameter content --" << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerSetSingleParameter::totalLog() const
{
    NetAnswerCommandDone::totalLog();
    NetAnswerSetSingleParameter::log();
}