using System;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;

using Icarus.Authorization;

namespace Icarus.Authorization.Handlers
{
    public class HasScopeHandler : AuthorizationHandler<HasScopeRequirement>
    {
        protected override Task HandleRequirementAsync(AuthorizationHandlerContext context, HasScopeRequirement requirement)
        {
            var issuer = requirement.Issuer;
            var scope = requirement.Scope;
            var claim = string.Empty;
            // if (!context.User.HasClaim(c => c.Type == "scope" && c.Issuer == requirement.Issuer))
            if (!context.User.HasClaim(c => 
            {
                var i = 0;
                var b = 99;
                claim = c.Type;
                return c.Type == "scope";
        })
        )
            {
                return Task.CompletedTask;
            }

            var scopes = context.User.FindFirst(c =>
                c.Type == "scope" && c.Issuer == requirement.Issuer).Value.Split(' ');

            if (scopes.Any(s => s == requirement.Scope))
            {
                context.Succeed(requirement);
            }

            return Task.CompletedTask;
        }
    }
}
