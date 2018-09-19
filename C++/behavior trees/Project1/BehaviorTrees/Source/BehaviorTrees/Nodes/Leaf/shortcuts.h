#pragma once

#define GET_GO nodedata_ptr->GetAgentData().GetGameObject()
#define GET_BODY nodedata_ptr->GetAgentData().GetGameObject()->GetBody()
#define INIT_LOCAL_BB(TYPE) nodedata_ptr->InitialLocalBlackBoard<TYPE>()
#define GET_LOCAL_BB(TYPE) nodedata_ptr->GetLocalBlackBoard<TYPE>()