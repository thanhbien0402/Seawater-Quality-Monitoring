
import React from 'react'
import { AuthForm } from '../../components/forms/AuthForm'
// import readUserSession from '@/actions'
import { redirect } from 'next/navigation'
import { Icons } from '@/components/icons'
import SplineComponent from '@/components/SplineComponent'

export default async function page() {
  // const { data } = await readUserSession()
  // if (data.session) {
  //   return redirect('/')
  // }
  return (
    <>
      <>
        <div className="container relative h-screen flex-col items-center justify-center grid lg:max-w-none lg:grid-cols-2 lg:px-0">
          <div className="relative hidden h-full flex-col bg-muted p-10 text-white lg:flex dark:border-r">
            <div className="absolute inset-0 bg-gradient-to-t from-[#111627] to-[#344378]" />
            <div className="relative z-20 flex items-center text-lg font-medium">
              <Icons.sunset className="h-20 w-20" />
              Seawater Quality Monitor
            </div>
            <div className="relative z-20 mt-auto">
              {/* <div className="pointer-events-none touch-none overscroll-none absolute top-[-745px] w-[35vw] h-[50vh]"
                onWheel={(e) => e.preventDefault()}
                onTouchMove={(e) => e.preventDefault()}><SplineComponent /></div> */}
              <blockquote className="space-y-2">
                <p className="text-lg">
                  &ldquo;Get real-time seawater quality updates with our advanced monitoring system in Xuan Dai Bay, Phu Yen Province, ensuring you stay informed about key water conditions and make timely decisions to protect the marine environment.&rdquo;
                </p>
                <footer className="text-sm pt-3">
                  Made with ♡ by Nghia Ngo
                </footer>
              </blockquote>
            </div>
          </div>
          <div className="lg:p-8">
            <div className="mx-auto flex w-full flex-col justify-center space-y-6 sm:w-[350px]">
              <div className="flex flex-col space-y-2 text-center">
                <h1 className="text-2xl font-semibold tracking-tight">
                  Welcome back!
                </h1>
                <p className="text-sm text-muted-foreground">
                  Please enter your credentials to log in. New here? You can
                  also register.
                </p>
                <AuthForm />
              </div>
            </div>
          </div>
        </div>
      </>
    </>
  )
}
