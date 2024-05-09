#include <iostream>
#include <nftables/libnftables.h>
#include <sstream>
#include "nftables.h"

namespace http
{
    int setLoadBalancer(std::vector<std::string> ips)
    {
        struct nft_ctx *ctx;
        int ret;

        ctx = nft_ctx_new(NFT_CTX_DEFAULT);
        if (!ctx)
        {
            std::cerr << "Failed to allocate nftables context\n";
            return 1;
        }

        const char *nft_flush_command = "flush ruleset";
        const char *nft_table_command = "add table nat";
        const char *nft_chain_command = "add chain nat prerouting { type nat hook prerouting priority 0 ; }";

        std::ostringstream ss;
        ss << "add rule nat prerouting dnat to numgen inc mod 2 map {";
        for (int i = 0; i < ips.size(); i++)
        {
            ss << " " << i << " : " << ips[i] << ",";
        }
        ss << "}";
        std::string s = ss.str();
        const char *nft_dnat_rule_command = s.c_str();

        ret = nft_run_cmd_from_buffer(ctx, nft_flush_command);
        if (ret < 0)
        {
            std::cerr << "Failed to execute nftables flush command\n";
            nft_ctx_free(ctx);
            return 1;
        }

        ret = nft_run_cmd_from_buffer(ctx, nft_table_command);
        if (ret < 0)
        {
            std::cerr << "Failed to execute nftables table creation command\n";
            nft_ctx_free(ctx);
            return 1;
        }

        ret = nft_run_cmd_from_buffer(ctx, nft_chain_command);
        if (ret < 0)
        {
            std::cerr << "Failed to execute nftables chain creation command\n";
            nft_ctx_free(ctx);
            return 1;
        }

        ret = nft_run_cmd_from_buffer(ctx, nft_dnat_rule_command);
        if (ret < 0)
        {
            std::cerr << "Failed to execute nftables DNAT rule command\n";
            nft_ctx_free(ctx);
            return 1;
        }

        ret = nft_ctx_buffer_output(ctx);
        if (ret < 0)
        {
            std::cerr << "Failed to buffer nftables output\n";
            nft_ctx_free(ctx);
            return 1;
        }

        const char *output = nft_ctx_get_output_buffer(ctx);
        std::cout << "Output: " << output << std::endl;

        nft_ctx_unbuffer_output(ctx);
        nft_ctx_free(ctx);

        std::cout << "Commands performed successfully\n";
        return 0;
    }
}
