#include <string>
#include <future>
#include <thread>

#include "rulematch.h"
#include "geoip.h"
#include "misc.h"
#include "logger.h"
#include "nodeinfo.h"

void getTestFile(nodeInfo &node, std::string proxy, std::vector<downloadLink> &downloadFiles, std::vector<linkMatchRule> &matchRules, std::string defaultTestFile)
{
    writeLog(LOG_TYPE_RULES, "Rule match started.");
    std::string def_test_file = defaultTestFile;
    std::vector<downloadLink>::iterator iterFile = downloadFiles.begin();
    std::vector<linkMatchRule>::iterator iterRule = matchRules.begin();
    string_array::iterator iterRuleDetail;
    geoIPInfo outbound = node.outboundGeoIP.get();

    //scan the URLs first to find the default one
    writeLog(LOG_TYPE_RULES, "Searching default rule.");
    while(iterFile != downloadFiles.end())
    {
        if(iterFile->tag == "Default")
        {
            def_test_file = iterFile->url;
        }
        iterFile++;
    }
    writeLog(LOG_TYPE_RULES, "Using default rule: '" + def_test_file + "'.");

    //only need to match outbound address
    while(iterRule != matchRules.end())
    {
        if(iterRule->mode == "match_isp")
        {
            iterRuleDetail = iterRule->rules.begin();
            while(iterRuleDetail != iterRule->rules.end())
            {
                if(outbound.organization == *iterRuleDetail)
                {
                    iterFile = downloadFiles.begin();
                    while(iterFile != downloadFiles.end())
                    {
                        if(iterFile->tag == iterRule->tag)
                        {
                            node.testFile = iterFile->url;
                            writeLog(LOG_TYPE_RULES, "Node  " + node.group + " - " + node.remarks + "  matches ISP rule '" + iterRule->tag + "'.");
                            break;
                        }
                        iterFile++;
                    }
                }
                iterRuleDetail++;
            }
        }
        else if(iterRule->mode == "match_country")
        {
            iterRuleDetail = iterRule->rules.begin();
            while(iterRuleDetail != iterRule->rules.end())
            {
                if(outbound.country == *iterRuleDetail || outbound.country_code == *iterRuleDetail)
                {
                    iterFile = downloadFiles.begin();
                    while(iterFile != downloadFiles.end())
                    {
                        if(iterFile->tag == iterRule->tag)
                        {
                            node.testFile = iterFile->url;
                            writeLog(LOG_TYPE_RULES, "Node  " + node.group + " - " + node.remarks + "  matches country rule '" + iterRule->tag + "'.");
                            break;
                        }
                        iterFile++;
                    }
                }
                iterRuleDetail++;
            }
        }
        else if(iterRule->mode == "match_group")
        {
            iterRuleDetail = iterRule->rules.begin();
            while(iterRuleDetail != iterRule->rules.end())
            {
                if(node.group == *iterRuleDetail)
                {
                    iterFile = downloadFiles.begin();
                    while(iterFile != downloadFiles.end())
                    {
                        if(iterFile->tag == iterRule->tag)
                        {
                            node.testFile = iterFile->url;
                            writeLog(LOG_TYPE_RULES, "Node  " + node.group + " - " + node.remarks + "  matches group rule '" + iterRule->tag + "'.");
                            break;
                        }
                        iterFile++;
                    }
                }
                iterRuleDetail++;
            }
        }
        iterRule++;
    }
    if(node.testFile.empty()) //no match rule
    {
        writeLog(LOG_TYPE_RULES, "Node  " + node.group + " - " + node.remarks + "  matches no rule. Using default rule.");
        node.testFile = def_test_file;
    }
    writeLog(LOG_TYPE_RULES, "Node  " + node.group + " - " + node.remarks + "  uses test file '" + node.testFile +"'.");
}
