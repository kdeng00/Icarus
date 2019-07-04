using System;

using NLog;

namespace Icarus.Controllers.Managers
{
    public class BaseManager
    {
    	#region Fields
    	protected static Logger _logger = NLog.Web.NLogBuilder.ConfigureNLog("nlog.config").GetCurrentClassLogger();
    	#endregion
    }
}
