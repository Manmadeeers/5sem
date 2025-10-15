using Microsoft.AspNetCore.Identity;
using System.Threading.Tasks;

namespace BSTU.Results.Authenticate.Services
{
    public class AuthenticationService
    {
        private readonly UserManager<IdentityUser> _userManager;
        private SignInManager<IdentityUser> _signInManager;


        public AuthenticationService(UserManager<IdentityUser> userManager, SignInManager<IdentityUser> signInManager)
        {
            _userManager = userManager;
            _signInManager = signInManager;
        }

        public async Task<SignInResult>SignInAsync(string username, string password)
        {
            var user = await _userManager.FindByNameAsync(username);
            if(user != null)
            {
                return await _signInManager.PasswordSignInAsync(user, password, false, false);
            }
            return SignInResult.Failed;
        }

        public async Task SignOut()
        {
            await _signInManager.SignOutAsync();
        }

    }   
}
