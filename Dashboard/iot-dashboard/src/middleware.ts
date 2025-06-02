import { createServerClient, type CookieOptions } from '@supabase/ssr'
import { NextResponse, type NextRequest } from 'next/server'

// Danh sách route cho phép (không cần redirect)
const PUBLIC_PATHS = ['/auth', '/map']
// Regex cho /dashboard/:deviceId
const DASHBOARD_ID = /^\/dashboard\/[^\/]+$/

export async function middleware(request: NextRequest) {
  let response = NextResponse.next({
    request: {
      headers: request.headers
    }
  })

  const supabase = createServerClient(
    process.env.NEXT_PUBLIC_SUPABASE_URL!,
    process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY!,
    {
      cookies: {
        get(name: string) {
          return request.cookies.get(name)?.value
        },
        set(name: string, value: string, options: CookieOptions) {
          request.cookies.set({
            name,
            value,
            ...options
          })
          response = NextResponse.next({
            request: {
              headers: request.headers
            }
          })
          response.cookies.set({
            name,
            value,
            ...options
          })
        },
        remove(name: string, options: CookieOptions) {
          request.cookies.set({
            name,
            value: '',
            ...options
          })
          response = NextResponse.next({
            request: {
              headers: request.headers
            }
          })
          response.cookies.set({
            name,
            value: '',
            ...options
          })
        }
      }
    }
  )

  const {
    data: { user }
  } = await supabase.auth.getUser()

  const { pathname } = request.nextUrl

  // Nếu đang vào /auth
  if (pathname === '/auth') {
    if (user) {
      // đã login → ép về /map
      return NextResponse.redirect(new URL('/map', request.url))
    }
    // chưa login → giữ nguyên ở /auth
    return response
  }

  // Nếu chưa login và không phải /auth → redirect về /auth
  if (!user) {
    return NextResponse.redirect(new URL('/auth', request.url))
  }

  // Ở đây chắc chắn user đã login

  // Nếu vào / hoặc /dashboard (chưa có id) → ép về /map
  if (pathname === '/' || pathname === '/dashboard') {
    return NextResponse.redirect(new URL('/map', request.url))
  }

  // Nếu vào /map hoặc /dashboard/:deviceId → cho qua
  if (PUBLIC_PATHS.includes(pathname) || DASHBOARD_ID.test(pathname)) {
    return response
  }

  // Mọi URL khác → ép về /map
  return NextResponse.redirect(new URL('/map', request.url))
}

export const config = {
  matcher: [
    /*
     * Match all request paths except for the ones starting with:
     * - _next/static (static files)
     * - _next/image (image optimization files)
     * - favicon.ico (favicon file)
     * Feel free to modify this pattern to include more paths.
     */
    '/((?!_next/static|_next/image|favicon.ico|.*\\.(?:svg|png|jpg|jpeg|gif|webp)$).*)'
  ]
}
