using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using NLog.Web;

namespace Icarus
{
	public class Program
    	{
        	public static void Main(string[] args)
        	{
			var logger = NLog.Web.NLogBuilder.ConfigureNLog("nlog.config").GetCurrentClassLogger();

			try
			{
				logger.Debug("init main");
            			CreateWebHostBuilder(args).Build().Run();
			}
			catch (Exception ex)
			{
				logger.Error(ex, "An error occurred");
				throw;
			}
			finally
			{
				NLog.LogManager.Shutdown();
			}
        	}

        	public static IWebHostBuilder CreateWebHostBuilder(string[] args) =>
            		WebHost.CreateDefaultBuilder(args)
                	.UseStartup<Startup>()
			.UseUrls("http://localhost:5002")
			.ConfigureLogging(logging =>
			{
				logging.ClearProviders();
				logging.SetMinimumLevel(Microsoft.Extensions.Logging.LogLevel.Trace);
			})
			.UseNLog() ;
    }
}
