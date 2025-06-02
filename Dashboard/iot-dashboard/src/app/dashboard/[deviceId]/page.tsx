import { redirect } from 'next/navigation'

import readUserSession, { getDeviceInfo } from '@/actions'
import { Header } from '@/components/Header'
import { Footer } from '@/components/Footer'
import { MapInterface } from '@/components/MapInterface'
import { Navbar } from '@/components/Navbar'
import SplineComponent from '@/components/SplineComponent'

interface PageProps {
  params: { deviceId: string }
}

export default async function Home({ params }: PageProps) {
  const { data } = await readUserSession()

  if (!data.session) {
    return redirect('/auth')
  }

  return (
    <div className="bg-gradient-to-t from-[#111627] to-[#344378] h-screen">
      <div className="absolute w-full h-full text-white flex flex-col justify-between">
        <div className="flex flex-col">
          <Navbar deviceId={params.deviceId} />
          <Header />
        </div>
        <Footer deviceId={params.deviceId} />
      </div>
      <SplineComponent />
    </div>
  )
}
