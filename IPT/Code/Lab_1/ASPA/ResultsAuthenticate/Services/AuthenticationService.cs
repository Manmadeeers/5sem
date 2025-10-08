using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Identity;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BRTU.ResultsAuthenticate.Services
{
    public class AuthenticationService
    {
        private readonly UserManager<IdentityUser> _userManager;
        private readonly IHttpContextAccessor httpContextAccessor;

    }
}
